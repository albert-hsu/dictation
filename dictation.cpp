/****************************************************************************
**
** Copyright (C) 2019 Albert Hsu
**
** This file is part of Dictation.
**
** Dictation is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Dictation is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Dictation.  If not, see <https://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifdef  LOG
#error  "LOG should not be defined"
#else
#ifdef  DICTATION_DEBUG
#define LOG qDebug()<<Q_FUNC_INFO<<"$"
#else
#define LOG NullDebug()
#endif
#endif

#include "dictation.h"
#include "utils/debug.h"
#include "utils/color.h"
#include "utils/md5.h"

#include <QStyleFactory>
#include <QApplication>
#include <QToolTip>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QShortcut>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QFileInfo>
#include <QRegularExpression>

Dictation::Dictation(QWidget* parent): QMainWindow(parent)
{
    if ( ! QStyleFactory::keys().contains("Fusion", Qt::CaseInsensitive))
        qFatal("Fusion style cannot be applied");
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    setFixedWidth(500);
    setMinimumHeight(600);

    // Set up tooltip styling
    {
        QPalette P = QToolTip::palette();
        P.setColor(QPalette::ToolTipBase, Color::Dark);
        P.setColor(QPalette::ToolTipText, Color::Light);
        QToolTip::setPalette(P);
    }
    // Set up global styling
    {
        QPalette P = palette();
        P.setColor(QPalette::WindowText, Color::Dark);
        P.setColor(QPalette::Window, Color::Light);
        setPalette(P);
    }

    // Set up fonts
    {
        int fontId;
        fontId = QFontDatabase::addApplicationFont(":/fonts/open-sans/OpenSans-Semibold.ttf");
        if (fontId == -1)
            qCritical() << "Opensans-Semibold.ttf cannot be load";
        fontId = QFontDatabase::addApplicationFont(":/fonts/Merriweather/Merriweather-Italic.ttf");
        if (fontId == -1)
            qCritical() << "Merriweather-Italic.ttf cannot be load";

        qApp->setFont(QFontDatabase().font("Open Sans", "Semibold", 12));
    }

    const QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    Q_ASSERT( ! path.isEmpty());
    m_appData.setPath(path);
    LOG << "AppData" << m_appData.absolutePath();

    m_player = new AudioPlayer(this);
    m_segmenter = new SpeechSegmenter(this);
    m_controlPanel = new ControlPanel(m_player, this);
    m_segmentPanel = new SegmentPanel(this);

    CONNECT(m_segmentPanel, &SegmentPanel::startAbRepeat, m_player, &AudioPlayer::startAbRepeat);
    CONNECT(m_segmentPanel, &SegmentPanel::stopAbRepeat, m_player, &AudioPlayer::stopAbRepeat);
    CONNECT(m_segmentPanel, &SegmentPanel::setPlaybackPosition, m_player, &AudioPlayer::setPosition);
    CONNECT(m_segmentPanel, &SegmentPanel::availableChanged, this, &Dictation::onSegmentPanelAvailableChange);
    CONNECT(m_player, &AudioPlayer::abRepeatStopped, m_segmentPanel, &SegmentPanel::onAbRepeatStop);
    CONNECT(m_player, &AudioPlayer::mediaStatusChanged, this, &Dictation::onPlayerStatusChange);
    CONNECT(m_player, &AudioPlayer::positionChanged, m_segmentPanel, &SegmentPanel::updatePlaybackPosition);
    CONNECT(m_player, static_cast<void (AudioPlayer::*)(AudioPlayer::Error)>(&AudioPlayer::error), this, &Dictation::onPlayerError);
    CONNECT(m_segmenter, &SpeechSegmenter::segmentsChanged, m_segmentPanel, &SegmentPanel::loadIntervals);
    CONNECT(m_segmenter, &SpeechSegmenter::error, this, &Dictation::onSegmenterError);
    CONNECT(m_segmenter, &SpeechSegmenter::durationChanged, m_controlPanel, &ControlPanel::setDuration);
    CONNECT(m_segmenter, &SpeechSegmenter::durationChanged, this, &Dictation::onDurationChange);
    CONNECT(m_controlPanel, &ControlPanel::previous, m_segmentPanel, &SegmentPanel::playPrevious);
    CONNECT(m_controlPanel, &ControlPanel::next, m_segmentPanel, &SegmentPanel::playNext);

    // Set up central widget
    QWidget* central = new QWidget;
    {
        QVBoxLayout* C = new QVBoxLayout;
        C->addWidget(m_controlPanel);
        C->addWidget(m_segmentPanel);
        central->setLayout(C);
    }
    setCentralWidget(central);

    // Set up keyboard shortcuts
    const QShortcut* showCurrent = new QShortcut(Qt::Key_F1, this);
    const QShortcut* focusPrevious = new QShortcut(Qt::CTRL + Qt::Key_PageUp, this);
    const QShortcut* focusNext = new QShortcut(Qt::CTRL + Qt::Key_PageDown, this);
    const QShortcut* togglePlayPause = new QShortcut(Qt::CTRL + Qt::Key_P, this);
    const QShortcut* playPrevious = new QShortcut(Qt::CTRL + Qt::Key_B, this);
    const QShortcut* playNext = new QShortcut(Qt::CTRL + Qt::Key_F, this);
    const QShortcut* toggleAbRepeat = new QShortcut(Qt::CTRL + Qt::Key_R, this);
    const QShortcut* playFocusedSegment = new QShortcut(Qt::CTRL + Qt::Key_M, this);
    CONNECT(showCurrent, &QShortcut::activated, m_segmentPanel, &SegmentPanel::showCurrent);
    CONNECT(focusPrevious, &QShortcut::activated, m_segmentPanel, &SegmentPanel::focusPrevious);
    CONNECT(focusNext, &QShortcut::activated, m_segmentPanel, &SegmentPanel::focusNext);
    CONNECT(togglePlayPause, &QShortcut::activated, m_player, &AudioPlayer::togglePlayPause);
    CONNECT(playPrevious, &QShortcut::activated, m_segmentPanel, &SegmentPanel::playPrevious);
    CONNECT(playNext, &QShortcut::activated, m_segmentPanel, &SegmentPanel::playNext);
    CONNECT(toggleAbRepeat, &QShortcut::activated, m_segmentPanel, &SegmentPanel::toggleAbRepeat);
    CONNECT(playFocusedSegment, &QShortcut::activated, m_segmentPanel, &SegmentPanel::playFocusedSegment);

    // Set up actions & toolbar
    QIcon openIcon;
    const QPixmap openPix(":/folder.png");
    openIcon.addPixmap(Color::fillColor(openPix, Color::Light), QIcon::Normal, QIcon::On);
    openIcon.addPixmap(Color::fillColor(openPix, Color::ButtonHovered), QIcon::Active, QIcon::On);
    QAction* openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an audio file."));
    CONNECT(openAct, &QAction::triggered, this, &Dictation::open);

    QIcon saveAsIcon;
    const QPixmap saveAsPix(":/export.png");
    saveAsIcon.addPixmap(Color::fillColor(saveAsPix, Color::Light), QIcon::Normal, QIcon::On);
    saveAsIcon.addPixmap(Color::fillColor(saveAsPix, Color::ButtonHovered), QIcon::Active, QIcon::On);
    saveAsIcon.addPixmap(Color::fillColor(saveAsPix, Color::ButtonDisabled), QIcon::Disabled, QIcon::On);
    m_saveAsAct = new QAction(saveAsIcon, tr("&Save as..."), this);
    m_saveAsAct->setEnabled(false);
    m_saveAsAct->setShortcuts(QKeySequence::SaveAs);
    m_saveAsAct->setStatusTip(tr("Export as a text file."));
    CONNECT(m_saveAsAct, &QAction::triggered, this, &Dictation::saveAs);

    QToolBar* toolbar = new QToolBar(this);
    toolbar->setMovable(false);
    toolbar->addAction(openAct);
    toolbar->addAction(m_saveAsAct);
    {
        QPalette P = toolbar->palette();
        P.setColor(QPalette::Window, Color::Dark);
        P.setColor(QPalette::Button, Color::Dark);
        toolbar->setPalette(P);
    }
    addToolBar(Qt::TopToolBarArea, toolbar);

    QStatusBar* statusbar = new QStatusBar;
    setStatusBar(statusbar);

    loadSettings();
}

void Dictation::clear()
{
    if (m_segmenter->isRunning()) {
        m_segmenter->terminate();
        if ( ! m_segmenter->wait(1000))
            qWarning() << "Unable to terminate the thread which runs the segmenter";
    }

    m_player->clear();
    m_controlPanel->setPanelEnabled(false);
    m_segmentPanel->clear();

    m_audioFileName.clear();
    toggleLoading(false);
}

void Dictation::open()
{
    const QString dir = m_audioFileName.isEmpty() ?
        QStandardPaths::writableLocation(QStandardPaths::MusicLocation) :
        QFileInfo(m_audioFileName).path();
    const QString audioFileName = QFileDialog::getOpenFileName(
        this,
        tr("Open"),
        dir,
        tr("Audio (*.mp3 *.wav *.wma)"),
        nullptr,
        // NOTE: A workaround for QTBUG-51712
        QFileDialog::DontUseNativeDialog
    );
    if (audioFileName.isNull())
        LOG << "No file chosen";
    else
        _open(audioFileName);
}

void Dictation::saveAs()
{
    Q_ASSERT( ! m_audioFileName.isEmpty());

    // NOTE: If the audio file named abc.mp3, the default output file will be abc.srt
    const QString defaultFileName = m_audioFileName.left(m_audioFileName.lastIndexOf(QChar('.'))).append(QStringLiteral(".srt"));

    const QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save as"),
        defaultFileName,
        tr("SubRip (*.srt)"),
        nullptr,
        QFileDialog::DontUseNativeDialog
    );
    if (fileName.isNull())
        LOG << "No file chosen";
    else
        _saveAs(fileName);
}

void Dictation::_saveAs(const QString& fileName)
{
    LOG << fileName;
    m_segmentPanel->saveAs(fileName);
}

void Dictation::closeEvent(QCloseEvent* event)
{
    if (isValidSrtFileName(m_srtFileName))
        _saveAs(m_appData.filePath(m_srtFileName));
    storeSettings();
    event->accept();
}

bool Dictation::isValidSrtFileName(const QString& fileName)
{
    if (fileName.isEmpty() || fileName.contains(QChar('*')))
        return false;
    Q_ASSERT(QRegularExpression("^[0-9a-f]{32}-[1-9]\\d*\\.srt$").match(fileName).hasMatch());
    return true;
}

void Dictation::loadSettings()
{
    Q_ASSERT(m_settings == nullptr);

    m_settings = new QSettings(m_appData.filePath("settings.ini"), QSettings::IniFormat, this);
    m_controlPanel->setVolume(m_settings->value("volume", 50).toInt());
    m_controlPanel->setMuted(m_settings->value("muted", false).toBool());

    const QString audioFileName = m_settings->value("opened_file").toString();
    if (QFileInfo(audioFileName).isReadable())
        _open(audioFileName);
}

void Dictation::storeSettings()
{
    if ( ! m_settings) return;
    m_settings->setValue("volume", m_controlPanel->volume());
    m_settings->setValue("muted", m_controlPanel->muted());
    m_settings->setValue("opened_file", m_audioFileName);
}

void Dictation::_open(const QString& audioFileName)
{
    LOG << audioFileName;

    clear();
    m_audioFileName = audioFileName;
    m_player->setSourceFileName(audioFileName);

    const QString srtNameTemplate(Md5::md5File(audioFileName) + "-*.srt");
    LOG << srtNameTemplate;
    const QDir::Filters filters = QDir::Files | QDir::Readable | QDir::Writable;

    qint64 duration;
    bool ok;
    const QStringList names = m_appData.entryList(QStringList(srtNameTemplate), filters);
    switch (names.size()) {
        case 0:
            toggleLoading(true);
            m_srtFileName = srtNameTemplate;
            m_controlPanel->setAudioFileName(audioFileName);
            m_segmenter->setSourceFileName(audioFileName);
            m_segmenter->run();
            break;
        case 1:
            toggleLoading(true);
            if ( ! isValidSrtFileName((m_srtFileName = names.at(0)))) {
                clear();
                return;
            }
            duration = m_srtFileName.mid(33, m_srtFileName.size() - 37).toLongLong(&ok);
            if ( ! ok) {
                clear();
                return;
            }
            m_controlPanel->setAudioFileName(audioFileName);
            m_controlPanel->setDuration(duration);
            m_segmentPanel->open(m_appData.filePath(m_srtFileName));
            break;
        default:
            qWarning() << "Expected 0 or 1 file that matches the glob but found many.";
            clear();
            return;
    }
}

/*
void Dictation::resizeEvent(QResizeEvent* event)
{
    LOG << event;
    super::resizeEvent(event);
}
*/

// NOTE: A workaround to determine the availibility of audio content,
//  due to the misbehaving of signal QMediaPlayer::audioAvailableChanged(bool available)
//  that it does not emitted when calling QMediaPlayer::setMedia(QMediaContent());
void Dictation::onPlayerStatusChange(AudioPlayer::MediaStatus status)
{
    if (status == AudioPlayer::NoMedia)
        m_controlPanel->setPanelEnabled(false);
    else if (status == AudioPlayer::LoadedMedia)
        m_controlPanel->setPanelEnabled(true);
}

void Dictation::onPlayerError(AudioPlayer::Error error)
{
    if (error != AudioPlayer::NoError)
        clear();
}

void Dictation::onSegmentPanelAvailableChange(bool available)
{
    LOG << available;
    m_saveAsAct->setEnabled(available);
    if (available)
        toggleLoading(false);
}

void Dictation::onSegmenterError(SpeechSegmenter::Error error)
{
    LOG << error;
    if (error != SpeechSegmenter::NoError)
        clear();
}

void Dictation::onDurationChange(qint64 msec)
{
    m_srtFileName.replace(QStringLiteral("*"), QString::number(msec));
}

void Dictation::toggleLoading(bool isLoading)
{
    LOG << isLoading;
    if (m_isLoading == isLoading) return;
    if ((m_isLoading = isLoading))
        QApplication::setOverrideCursor(Qt::WaitCursor);
    else
        QApplication::restoreOverrideCursor();
}

#undef  LOG

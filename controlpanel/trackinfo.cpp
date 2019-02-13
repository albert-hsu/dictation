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
#ifdef  TRACKINFO_DEBUG
#define LOG qDebug()<<Q_FUNC_INFO<<"$"
#else
#define LOG NullDebug()
#endif
#endif

#include "trackinfo.h"
#include "utils/debug.h"

#include <QFontDatabase>
#include <QVBoxLayout>
#include <QDir>

TrackInfo::TrackInfo(AudioPlayer* player, QWidget* parent): super(parent), m_player(player)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_1stLine = new QLabel(this);
    m_1stLine->setAlignment(Qt::AlignHCenter);
    QFont F1 = QFontDatabase().font("Merriweather", "Italic", 19);
    m_1stLine->setFont(F1);

    m_2ndLine = new QLabel(this);
    m_2ndLine->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->addWidget(m_1stLine);
    vbox->addWidget(m_2ndLine);

    CONNECT(m_player, &AudioPlayer::metaDataAvailableChanged, this, &TrackInfo::updateTrackInfo);
}

void TrackInfo::updateTrackInfo(bool available)
{
    if (available) {
        const QStringList meta = m_player->availableMetaData();
        LOG << "metadata keys" << meta;

        QString name, artist, album;

        if (meta.contains(QMediaMetaData::Title))
            name = metaData(QMediaMetaData::Title);
        else
            // NOTE: QFileInfo::completeBaseName() does not work as expected
            name = m_audioFileName.split(QDir::separator()).last();
        if (name.isEmpty()) {
            clear();
            return;
        }
        setElideText(m_1stLine, name);

        if (meta.contains(QMediaMetaData::ContributingArtist))
            artist = metaData<QStringList>(QMediaMetaData::ContributingArtist).at(0);
        else
            artist = metaData(QMediaMetaData::AlbumArtist);

        album = metaData(QMediaMetaData::AlbumTitle);

        QStringList text;
        if ( ! artist.isEmpty()) text << artist;
        if ( ! album.isEmpty()) text << album;
        // NOTE: \u2014 is an em dash in Unicode
        setElideText(m_2ndLine, text.join(QStringLiteral(" \u2014 ")));
    } else {
        m_1stLine->clear();
        m_2ndLine->clear();
        m_audioFileName.clear();
    }
}

void TrackInfo::clear()
{
    updateTrackInfo(false);
}

template <typename T>
T TrackInfo::metaData(const QString& key)
{
    return m_player->metaData(key).value<T>();
}

void TrackInfo::setElideText(QLabel* label, const QString& text)
{
    const QFontMetrics metrix = label->fontMetrics();
    const int width = this->width() - 15;
    const QString clippedText = metrix.elidedText(text, Qt::ElideRight, width);
    label->setText(clippedText);
    label->setStatusTip(text);
}

QSize TrackInfo::sizeHint() const
{
    return QSize(220, 65);
}

#undef LOG

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

#ifndef __DICTATION_H__
#define __DICTATION_H__

#include "audioplayer/audioplayer.h"
#include "speechsegmenter/speechsegmenter.h"
#include "controlpanel/controlpanel.h"
#include "segmentpanel/segmentpanel.h"

#include <QMainWindow>
#include <QSettings>
#include <QAction>
#include <QString>
#include <QDir>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

class Dictation: public QMainWindow {
Q_OBJECT
    typedef QMainWindow super;
public:
    Dictation(QWidget* parent = 0);
signals:
    void current();
    void previous();
    void next();
private slots:
    void open();
    void saveAs();
    void clear();

    void onPlayerStatusChange(AudioPlayer::MediaStatus status);
    void onPlayerError(AudioPlayer::Error error);
    void onSegmentPanelAvailableChange(bool available);
    void onSegmenterError(SpeechSegmenter::Error error);
    void onDurationChange(qint64 msec);
protected:
    void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
    // void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
private:
    AudioPlayer* m_player;
    SpeechSegmenter* m_segmenter;
    ControlPanel* m_controlPanel;
    SegmentPanel* m_segmentPanel;

    void _open(const QString& audioFileName);
    void _saveAs(const QString& fileName);
    bool isValidSrtFileName(const QString& fileName);
    void loadSettings();
    void storeSettings();
    void toggleLoading(bool isLoading);

    QSettings* m_settings = nullptr;
    QAction* m_saveAsAct;
    QString m_audioFileName;
    // NOTE: m_srtFileName is the name of a .srt file. For example,
    //  it is as the form `85f6f150e30fe7e6aa53ba211bfed0e3-479843.srt`.
    //  A file name begins with an md5 hash, followed by a minus sign `-` and
    //  a decimal number which is the duration of the audio file in milliseconds.
    QString m_srtFileName;
    QDir m_appData;
    bool m_isLoading = false;
};

#endif

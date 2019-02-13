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

#ifndef __TRACKINFO_H__
#define __TRACKINFO_H__

#include "audioplayer/audioplayer.h"

#include <QWidget>
#include <QString>
#include <QMediaMetaData>
#include <QLabel>

class TrackInfo: public QWidget {
Q_OBJECT
    typedef QWidget super;
public:
    explicit TrackInfo(AudioPlayer* player, QWidget* parent);

    QSize sizeHint() const Q_DECL_OVERRIDE;
    void setAudioFileName(const QString& audioFileName) {m_audioFileName = audioFileName;}
public slots:
    void updateTrackInfo(bool available);
    void clear();
private:
    const AudioPlayer* m_player;
    QLabel* m_1stLine;
    QLabel* m_2ndLine;

    QString m_audioFileName;

    template <typename T = QString> T metaData(const QString& key);
    void setElideText(QLabel* label, const QString& text);
};

#endif

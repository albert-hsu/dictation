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

#ifndef __AUDIOPLAYER_H__
#define __AUDIOPLAYER_H__

#include <QMediaPlayer>
#include <QString>
#include <QWidget>

class AudioPlayer: public QMediaPlayer
{
Q_OBJECT
public:
    explicit AudioPlayer(QWidget* parent);

    void setSourceFileName(const QString& fileName);
    bool isRepeating() {return m_isRepeating;}
public slots:
    void startAbRepeat(qint64 msecA, qint64 msecB);
    void stopAbRepeat();
    void setPosition(qint64 msec);
    void togglePlayPause();
    void clear();
signals:
    void abRepeatStopped();
private slots:
    void abRepeat(qint64 msec);
    void onStateChange(State state);
protected:
    void moveTo(qint64 msec);
private:
    bool m_isRepeating = false;
    qint64 m_a, m_b;

    void _stopAbRepeat();
};

#endif

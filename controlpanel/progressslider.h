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

#ifndef __PROGRESSSLIDER_H__
#define __PROGRESSSLIDER_H__

#include <QWidget>
#include <QSlider>
#include <QLabel>

class ProgressSlider: public QWidget {
Q_OBJECT
public:
    explicit ProgressSlider(QWidget* parent);
signals:
    void playbackPositionChanged(qint64 msec);
public slots:
    void setDuration(qint64 msec);
    void setPosition(qint64 msec);
    void setSliderEnabled(bool enabled);
private slots:
    void triggerChanged(int action);
private:
    QSlider* m_slider;
    QLabel* m_positionL;
    QLabel* m_durationL;

    const qint64 m_Unit = 1000;
    QString m_durationTimecode;
    qint64 m_position;
    qint64 m_duration;

    QString toTimecode(qint64 msec, bool hourField);
};

#endif

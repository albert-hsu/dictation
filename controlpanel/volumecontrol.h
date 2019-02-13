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

#ifndef __VOLUMECONTROL_H__
#define __VOLUMECONTROL_H__

#include "customwidgets/button.h"
#include "customwidgets/tooltippedslider.h"

#include <QWidget>

class VolumeControl: public QWidget
{
Q_OBJECT
public:
    explicit VolumeControl(QWidget* parent);

    void setMuted(bool muted);
    void setVolume(int volume);
    bool muted() {return m_muted;}
    int volume() {return m_volume;}
signals:
    void mutedChanged(bool muted);
    void volumeChanged(int volume);
private slots:
    void triggerVolumeChanged(int action);
    void onIconClick();
private:
    enum {
        VolumeMuted = 0,
        Volume1 = 30,
        Volume2 = 70,
        VolumeMax = 100
    };

    Button* m_icon;
    TooltippedSlider* m_slider;

    bool m_muted;
    int m_volume;
    int m_iconX, m_icon0, m_icon1, m_icon2, m_icon3;

    void updateIcon();
};

#endif

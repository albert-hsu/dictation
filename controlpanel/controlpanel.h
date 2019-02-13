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

#ifndef __CONTROLPANEL_H__
#define __CONTROLPANEL_H__

#include "trackinfo.h"
#include "controlbuttonset.h"
#include "progressslider.h"
#include "volumecontrol.h"
#include "audioplayer/audioplayer.h"

#include <QWidget>
#include <QString>

class ControlPanel: public QWidget {
Q_OBJECT
    typedef QWidget super;
public:
    explicit ControlPanel(AudioPlayer* player, QWidget* parent);

    int volume() {return m_vControl->volume();}
    bool muted() {return m_vControl->muted();}
    void setVolume(int volume) {m_vControl->setVolume(volume);}
    void setMuted(bool muted) {m_vControl->setMuted(muted);}
    void setAudioFileName(const QString& fileName);
signals:
    void previous();
    void next();
private:
    AudioPlayer* m_player;
    TrackInfo* m_trackInfo;
    ControlButtonSet* m_buttonSet;
    ProgressSlider* m_pSlider;
    VolumeControl* m_vControl;
public slots:
    void setDuration(qint64 msec);
    void setPanelEnabled(bool enabled);
};

#endif

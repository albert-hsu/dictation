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
#ifdef  CONTROLPANEL_DEBUG
#define LOG qDebug()<<Q_FUNC_INFO<<"$"
#else
#define LOG NullDebug()
#endif
#endif

#include "controlpanel.h"
#include "utils/debug.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

ControlPanel::ControlPanel(AudioPlayer* player, QWidget* parent):
    super(parent),
    m_player(player)
{
    m_trackInfo = new TrackInfo(player, this);
    m_buttonSet = new ControlButtonSet(this);
    m_pSlider = new ProgressSlider(this);
    m_vControl = new VolumeControl(this);

    CONNECT(m_player, &AudioPlayer::stateChanged, m_buttonSet, &ControlButtonSet::setState);
    CONNECT(m_buttonSet, &ControlButtonSet::play, m_player, &AudioPlayer::play);
    CONNECT(m_buttonSet, &ControlButtonSet::pause, m_player, &AudioPlayer::pause);
    CONNECT(m_buttonSet, &ControlButtonSet::previous, this, &ControlPanel::previous);
    CONNECT(m_buttonSet, &ControlButtonSet::next, this, &ControlPanel::next);

    CONNECT(m_pSlider, &ProgressSlider::playbackPositionChanged, m_player, &AudioPlayer::setPosition);
    CONNECT(m_player, &AudioPlayer::positionChanged, m_pSlider, &ProgressSlider::setPosition);

    CONNECT(m_vControl, &VolumeControl::mutedChanged, m_player, &AudioPlayer::setMuted);
    CONNECT(m_vControl, &VolumeControl::volumeChanged, m_player, &AudioPlayer::setVolume);

    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->setContentsMargins(0, 0, 0, 0);
    hbox->addWidget(m_buttonSet);
    hbox->addStretch(1);
    hbox->addWidget(m_trackInfo);
    hbox->addStretch(1);
    hbox->addWidget(m_vControl);

    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->addLayout(hbox);
    vbox->addWidget(m_pSlider);

    setPanelEnabled(false);
}

void ControlPanel::setDuration(qint64 msec)
{
    m_pSlider->setDuration(msec);
}

void ControlPanel::setPanelEnabled(bool enabled)
{
    if ( ! enabled) {
        m_trackInfo->clear();
        m_pSlider->setSliderEnabled(false);
    }
    m_buttonSet->setEnabled(enabled);
}

void ControlPanel::setAudioFileName(const QString& fileName)
{
    m_trackInfo->setAudioFileName(fileName);
}

#undef  LOG

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
#ifdef  VOLUMECONTROL_DEBUG
#define LOG qDebug()<<Q_FUNC_INFO<<"$"
#else
#define LOG NullDebug()
#endif
#endif

#include "volumecontrol.h"
#include "utils/debug.h"
#include "utils/color.h"

#include <QHBoxLayout>

VolumeControl::VolumeControl(QWidget* parent): QWidget(parent)
{
    m_icon = new Button(QPixmap(":/speaker-0.png"), this);

    m_icon->setColor(Color::Dark,
        IconRole::Initial);
    m_icon->setColor(Color::ButtonHovered,
        IconRole::Hovered);
    m_icon->setColor(Color::ButtonPressed,
        IconRole::Hovered | IconRole::Pressed);

    m_icon0 = Button::OriginalIcon;
    m_icon1 = m_icon->addIcon(QPixmap(":/speaker-1.png"));
    m_icon2 = m_icon->addIcon(QPixmap(":/speaker-2.png"));
    m_icon3 = m_icon->addIcon(QPixmap(":/speaker-3.png"));
    m_iconX = m_icon->addIcon(QPixmap(":/speaker-x.png"));

    m_icon->setIconSize(QSize(25, 25));
    CONNECT(m_icon, &QAbstractButton::clicked, this, &VolumeControl::onIconClick);

    m_slider = new TooltippedSlider(Qt::Horizontal, this);
    const QString styleSheet = QStringLiteral(" \
        QSlider:horizontal{ \
            max-height:100px; \
        } \
        QSlider::groove:horizontal{ \
            border:0px; \
            height:6px; \
        } \
        QSlider::sub-page:horizontal{ \
            background:%1; \
        } \
        QSlider::add-page:horizontal{ \
            background:%2; \
        } \
        QSlider::handle:horizontal{ \
            background:%3; \
            width:10px; \
            border-radius:4px; \
            margin:-8px 0px; \
        }")
        .arg(QColor(Color::SliderSubPage).name())
        .arg(QColor(Color::SliderAddPage).name())
        .arg(QColor(Color::SliderHandle).name());

    m_slider->setStyleSheet(styleSheet);
    m_slider->setRange(VolumeMuted, VolumeMax);
    CONNECT(m_slider, &QAbstractSlider::actionTriggered, this, &VolumeControl::triggerVolumeChanged);

    QHBoxLayout* hbox = new QHBoxLayout(this);
    hbox->addWidget(m_icon);
    hbox->addWidget(m_slider);

    setContentsMargins(0, 0, 0, 0);
}

void VolumeControl::setMuted(bool muted)
{
    m_muted = muted;
    // NOTE: setSliderPosition(int) triggers QAbstractSlider::SliderAction SliderMove,
    //  setValue(int) does not
    m_slider->setValue(muted ? VolumeMuted : m_volume);
    updateIcon();
    emit mutedChanged(muted);
    LOG << "mutedChanged" << muted;
}

void VolumeControl::triggerVolumeChanged(int /*action*/)
{
    m_volume = m_slider->sliderPosition();
    if (m_muted && m_volume != VolumeMuted)
        setMuted(false);
    else
        updateIcon();
    emit volumeChanged(m_volume);
    LOG << "volumeChanged" << m_volume;
}

void VolumeControl::setVolume(int volume)
{
    Q_ASSERT(volume >= VolumeMuted && volume <= VolumeMax);

    m_volume = volume;
    setMuted(false);
    triggerVolumeChanged(QAbstractSlider::SliderNoAction);
}

void VolumeControl::updateIcon()
{
    Q_ASSERT(m_volume >= VolumeMuted && m_volume <= VolumeMax);

    m_icon->setStatusTip(m_muted ? tr("Unmute.") : tr("Mute."));

    if (m_muted)
        m_icon->swapIcon(m_iconX);
    else if (m_volume == VolumeMuted)
        m_icon->swapIcon(m_icon0);
    else if (m_volume < Volume1)
        m_icon->swapIcon(m_icon1);
    else if (m_volume < Volume2)
        m_icon->swapIcon(m_icon2);
    else if (m_volume <= VolumeMax)
        m_icon->swapIcon(m_icon3);
    else
        Q_UNREACHABLE();
}

void VolumeControl::onIconClick()
{
    setMuted( ! m_muted);
}

#undef  LOG

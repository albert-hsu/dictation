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
#ifdef  PROGRESSSLIDER_DEBUG
#define LOG qDebug()<<Q_FUNC_INFO<<"$"
#else
#define LOG NullDebug()
#endif
#endif

#include "progressslider.h"
#include "utils/debug.h"
#include "utils/color.h"

#include <QHBoxLayout>

ProgressSlider::ProgressSlider(QWidget* parent): QWidget(parent)
{

    m_slider = new QSlider(Qt::Horizontal);
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
    CONNECT(m_slider, &QAbstractSlider::actionTriggered, this, &ProgressSlider::triggerChanged);

    m_positionL = new QLabel;
    m_durationL = new QLabel;
    // Testing the maximum width a timecode can be.
    // (1) The adding 1 is a reasonable value for fontMetrics::leftBearing()
    // (2) Assuming the 2 labels have the same fontMetrics
    const int width_ = m_positionL->fontMetrics().boundingRect("00:00").width() + 1;
    m_positionL->setFixedWidth(width_);
    m_durationL->setFixedWidth(width_);

    m_positionL->setAlignment(Qt::AlignRight);
    m_durationL->setAlignment(Qt::AlignRight);

    QHBoxLayout* hbox = new QHBoxLayout(this);
    hbox->setContentsMargins(0, 0, 0, 0);
    hbox->addWidget(m_positionL);
    hbox->addWidget(m_slider);
    hbox->addWidget(m_durationL);

    setSliderEnabled(false);
}

void ProgressSlider::setDuration(qint64 msec)
{
    LOG << msec;

    m_duration = msec;
    m_durationL->setText(toTimecode(msec, msec >= 3600000));
    setSliderEnabled(true);
    m_slider->setRange(0, m_duration / m_Unit);
    m_position = msec + (m_Unit<<2), setPosition(0); // Force an update
}

void ProgressSlider::setPosition(qint64 msec)
{
    Q_ASSERT(msec >= 0 && msec < 86400000);

    if (m_position / m_Unit != msec / m_Unit) {
        m_positionL->setText(toTimecode(msec, msec >= 3600000));
        m_slider->setValue(msec / m_Unit);
        m_position = msec;
    }
    // NOTE: QSlider::setSliderPosition(int) triggers QSlider::actionTriggered(
    //  QAbstractSlider::SliderAction SliderMove), QSlider::setValue(int) does not
}

QString ProgressSlider::toTimecode(qint64 msec, bool hourField)
{
    Q_ASSERT(msec >= 0 && msec < 86400000);

    const qint64 hour = msec / 3600000;
    const qint64 min = msec / 60000 % 60;
    const qint64 sec = msec / 1000 % 60;

    const QChar zero('0');

    if (Q_UNLIKELY(hourField))
        return QStringLiteral("%1:%2:%3")
            .arg(hour)
            .arg(min, 2, 10, zero)
            .arg(sec, 2, 10, zero);
   else
        return QStringLiteral("%1:%2")
            .arg(min)
            .arg(sec, 2, 10, zero);
}

void ProgressSlider::triggerChanged(int /*action*/)
{
    const qint64 msec = m_slider->sliderPosition() * m_Unit;
    LOG << "playbackPositionChanged" << msec;
    emit playbackPositionChanged(msec);
}

void ProgressSlider::setSliderEnabled(bool enabled)
{
    if ( ! enabled) {
        m_positionL->setText("--:--");
        m_durationL->setText("--:--");
        m_slider->setValue(0);
    }
    m_slider->setEnabled(enabled);
}

#undef  LOG

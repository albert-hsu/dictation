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
#ifdef  TOOLTIPPEDSLIDER_DEBUG
#define LOG qDebug()<<Q_FUNC_INFO<<"$"
#else
#define LOG NullDebug()
#endif
#endif

#include "tooltippedslider.h"
#include "utils/debug.h"

#include <QStyleOptionSlider>
#include <QToolTip>

TooltippedSlider::TooltippedSlider(QWidget* parent): QSlider(parent)
{
}

TooltippedSlider::TooltippedSlider(Qt::Orientation orientation, QWidget* parent):
    QSlider(orientation, parent)
{
}

void TooltippedSlider::sliderChange(QAbstractSlider::SliderChange change)
{
    if (change == QAbstractSlider::SliderValueChange) {
        QStyleOptionSlider opt;
        initStyleOption(&opt);

        QRect handleRect(style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle));
        QToolTip::showText(mapToGlobal(handleRect.bottomLeft()), QString::number(value()));
    }
    QSlider::sliderChange(change);
}

#undef LOG

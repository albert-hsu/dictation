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
#ifdef  ROUNDBUTTON_DEBUG
#define LOG qDebug()<<Q_FUNC_INFO<<"$"
#else
#define LOG NullDebug()
#endif
#endif

#include "roundbutton.h"
#include "utils/debug.h"

#include <QRegion>

RoundButton::RoundButton(const QPixmap& image, QWidget* parent): super(image, parent)
{
}

void RoundButton::resizeEvent(QResizeEvent* event)
{
    const int width = event->size().width();
    const int height = event->size().height();
    const int side = qMin(width, height);
    const QRegion mask((width - side)/2 - 1, (height - side)/2 - 1, side+1, side+1, QRegion::Ellipse);
    setMask(mask);
}

#undef LOG

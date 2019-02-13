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

#ifndef __COLOR_H__
#define __COLOR_H__

#include <QColor>
#include <QPixmap>

namespace Color
{
    const int Turquoise = 0x1abc9c;
    const int GreenSea = 0x16a085;
    const int SunFlower = 0xf1c40f;
    const int Orange = 0xf39c12;
    const int Emerald = 0x2ecc71;
    const int Nephritis = 0x27ae60;
    const int Carrot = 0xe67e22;
    const int Pumpkin = 0xd35400;
    const int PeterRiver = 0x3498db;
    const int BelizeHole = 0x2980b9;
    const int Alizarin = 0xe74c3c;
    const int Pomegranate = 0xc0392b;
    const int Amethyst = 0x9b59b6;
    const int Wisteria = 0x8e44ad;
    const int Clouds = 0xecf0f1;
    const int Silver = 0xbdc3c7;
    const int WetAsphalt = 0x34495e;
    const int MidnightBlue = 0x2c3e50;
    const int Concrete = 0x95a5a6;
    const int Asbestos = 0x7f8c8d;

    const int White = 0xffffff;
    const int Black = 0x000000;
    const int Red = 0xff0000;
    const int Green = 0x00ff00;
    const int Blue = 0x0000ff;

    int toInt(const QColor color);
    QPixmap fillColor(const QPixmap& image, const QColor& color);

    const int Dark = MidnightBlue;
    const int Light = Clouds;
    const int ButtonHovered = Carrot;
    const int ButtonPressed = Pumpkin;
    const int ButtonDisabled = Concrete;
    const int SliderSubPage = BelizeHole;
    const int SliderAddPage = toInt(QColor(Light).darker(115));
    const int SliderHandle = toInt(QColor(Dark).lighter(130));
    const int ScrollBarBackground = SliderAddPage;
    const int ScrollBarHandle = SliderHandle;
};

#endif

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

#ifndef __TOOLTIPPEDSLIDER_H__
#define __TOOLTIPPEDSLIDER_H__

#include <QSlider>

class TooltippedSlider: public QSlider {
    Q_OBJECT
public:
    explicit TooltippedSlider(QWidget* parent = 0);
    explicit TooltippedSlider(Qt::Orientation orientation, QWidget* parent = 0);
protected:
    virtual void sliderChange(SliderChange change) Q_DECL_OVERRIDE;
};

#endif

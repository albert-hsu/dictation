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

#ifndef __ROUNDBUTTON_H__
#define __ROUNDBUTTON_H__

#include "button.h"

#include <QIcon>
#include <QPixmap>
#include <QResizeEvent>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

class RoundButton: public Button
{
    Q_OBJECT
    typedef Button super;
public:
    explicit RoundButton(const QPixmap& image, QWidget* parent = 0);
protected:
    void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
};

#endif

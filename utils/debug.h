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

#ifndef __UTILS_H__
#define __UTILS_H__

#include <QDebug>

#define CONNECT(sender, signal, receiver, slot) \
    do { \
        if ( ! connect(sender, signal, receiver, slot)) \
            qt_assert_x(Q_FUNC_INFO, "CONNECT failed", __FILE__, __LINE__); \
    } while (0)

#define CONNECT_5(sender, signal, receiver, slot, type) \
    do { \
        if ( ! connect(sender, signal, receiver, slot, type)) \
            qt_assert_x(Q_FUNC_INFO, "CONNECT_5 failed", __FILE__, __LINE__); \
    } while (0)

class NullDebug
{
public:
    template <typename T>
    NullDebug& operator<<(const T&) {return *this;}
};

inline NullDebug nullDebug() {return NullDebug();}

#endif

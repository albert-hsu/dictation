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
#ifdef  MAIN_DEBUG
#define LOG qDebug()<<Q_FUNC_INFO<<"$"
#else
#define LOG NullDebug()
#endif
#endif

#define XSTR(s) STR(s)
#define STR(s) #s
#define __APP_VERSION__ XSTR(__APP_VERSION_MAJOR__.__APP_VERSION_MINOR__)

#ifdef STANDALONE
#include "standalone.h"
#endif
#include "dictation.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("albert-hsu");
    QCoreApplication::setApplicationName("Dictation");
    QCoreApplication::setApplicationVersion(__APP_VERSION__);

    Dictation dictation;
    dictation.show();
    return app.exec();
}

#undef LOG

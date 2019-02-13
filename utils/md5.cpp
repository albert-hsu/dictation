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

#include "md5.h"

#include <stdexcept>
#include <QFile>
#include <QCryptographicHash>

QString Md5::md5File(const QString& fileName)
{
    QFile F(fileName);
    if (F.open(QIODevice::ReadOnly)) {
        const QByteArray data = F.readAll();
        const QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Md5);

        F.close();
        return hash.toHex();
    }
    throw std::runtime_error(qPrintable(F.errorString()));
}

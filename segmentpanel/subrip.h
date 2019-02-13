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

#ifndef __SUBRIP_H__
#define __SUBRIP_H__

#include <stdexcept>
#include <QTextStream>
#include <QObject>
#include <QVector>

struct Subtitle;
class SubRip;
class SubRipError;

class SubRipError: public std::exception
{
    const char* what() const throw() {return "SubRip: Format Error.";}
};

struct Subtitle
{
    Subtitle(qint64 begin, qint64 end, const QString& text=""):
        begin(begin), end(end), text(text) {}

    qint64 begin;
    qint64 end;
    QString text;
};

class SubRip: public QObject
{
Q_OBJECT
public:
    SubRip() {}
    ~SubRip();

    enum Error {
        FormatError
    };

    void clear();
    void setIntervals(const QVector<QPair<qint64, qint64> >& intervals);

    int size() const {return m_subtitles.size();}
    Subtitle* at(int i) const {return m_subtitles.at(i);}
    Subtitle* operator[](int i) const {return at(i);}
    Subtitle* operator[](int i) {return at(i);}

    friend QTextStream& operator>>(QTextStream& ts, SubRip& srt);

    static QString toTimecode(qint64 msec);
    static qint64 toMsec(const QString& timecode);
signals:
    void error(SubRip::Error);
    void finished();
private:
    QVector<Subtitle*> m_subtitles;
};

QTextStream& operator<<(QTextStream&, const SubRip&);
QTextStream& operator>>(QTextStream&, SubRip&);

#endif

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
#ifdef  SUBRIP_DEBUG
#define LOG qDebug()<<Q_FUNC_INFO<<"$"
#else
#define LOG NullDebug()
#endif
#endif

#include "subrip.h"
#include "utils/debug.h"

#include <QTime>
#include <QRegularExpression>

SubRip::~SubRip()
{
    clear();
}

void SubRip::clear()
{
    for (int i = 0; i < m_subtitles.size(); ++i) {
        delete m_subtitles.at(i);
    }
    m_subtitles.clear();
}

void SubRip::setIntervals(const QVector<QPair<qint64, qint64> >& intervals)
{
    if (m_subtitles.size()) clear();

    for (int i = 0; i < intervals.size(); ++i) {
        const QPair<qint64, qint64> time = intervals.at(i);

        m_subtitles.append(new Subtitle(time.first, time.second));
    }
    emit finished();
}

QString SubRip::toTimecode(qint64 msec)
{
    Q_ASSERT(msec < 86400000);

    const QString format("hh:mm:ss,zzz");
    const QTime time(msec/3600000, msec/60000%60, msec/1000%60, msec%1000);

    return time.toString(format);
}

qint64 SubRip::toMsec(const QString& timecode)
{
    // NOTE: hh:mm:ss,xxx
    const QVector<QStringRef> parts = timecode.splitRef(QRegularExpression(QStringLiteral("[:,]")));
    bool ok;
    qint64 msec;

    if (parts.size() != 4 ||
        parts.at(0).size() != 2 ||
        parts.at(1).size() != 2 ||
        parts.at(2).size() != 2 ||
        parts.at(3).size() != 3)
        throw SubRipError();
    msec = parts.at(3).toInt(&ok);
    if ( ! ok) throw SubRipError();
    msec += parts.at(2).toInt(&ok) * 1000;
    if ( ! ok) throw SubRipError();
    msec += parts.at(1).toInt(&ok) * 60000;
    if ( ! ok) throw SubRipError();
    msec += parts.at(0).toInt(&ok) * 3600000;
    if ( ! ok) throw SubRipError();

    return msec;
}

QTextStream& operator<<(QTextStream& out, const SubRip& srt)
{
    for (int i = 0; i < srt.size(); ++i) {
        const Subtitle* sub = srt.at(i);

        out << i+1 << '\n'
            << SubRip::toTimecode(sub->begin) << " --> "
            << SubRip::toTimecode(sub->end) << '\n';
        if ( ! sub->text.isEmpty())
            out << sub->text.trimmed() << '\n';
        out << '\n';
    }
    return out;
}

QTextStream& operator>>(QTextStream& ts, SubRip& srt)
{
    int count = 1;

    if (srt.size()) srt.clear();

    try {
        while ( ! ts.atEnd()) {
            if (ts.readLine() != QString::number(count++)) throw SubRipError();

            const QStringList tokens = ts.readLine().split(QStringLiteral(" "), QString::SkipEmptyParts);
            if (tokens.size() != 3) throw SubRipError();
            const qint64 begin = SubRip::toMsec(tokens.first());
            const qint64 end = SubRip::toMsec(tokens.last());
            if (tokens.at(1) != QStringLiteral("-->")) throw SubRipError();

            Subtitle* sub = new Subtitle(begin, end);
            while (1) {
                const QString line = ts.readLine();
                if (line.isNull()) {
                    throw SubRipError();
                } else if (line.isEmpty()) {
                    break;
                } else {
                    if ( ! sub->text.isEmpty()) sub->text += '\n';
                    sub->text += line;
                }
            }
            srt.m_subtitles.append(sub);
        }
        emit srt.finished();
    } catch (SubRipError& e) {
        srt.clear();
        emit srt.error(SubRip::FormatError);
    }
    return ts;
}

#undef  LOG

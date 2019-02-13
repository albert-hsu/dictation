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
#ifdef  SEGMENTPANEL_DEBUG
#define LOG qDebug()<<Q_FUNC_INFO<<"$"
#else
#define LOG NullDebug()
#endif
#endif

#include "segmentpanel.h"
#include "utils/debug.h"
#include "utils/color.h"

#include <limits>
#include <QScrollBar>
#include <QVBoxLayout>

SegmentPanel::SegmentPanel(QWidget* parent): super(parent)
{
    const QString styleSheet = QStringLiteral(" \
        QScrollBar:vertical{ \
            border:none; \
            background:%1; \
        } \
        QScrollBar::handle:vertical{ \
            background:%2; \
            border-radius:2px; \
        } \
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical{ \
            border:none; \
            background:none; \
            height:0px; \
        }")
        .arg(QColor(Color::ScrollBarBackground).name())
        .arg(QColor(Color::ScrollBarHandle).name());

    verticalScrollBar()->setStyleSheet(styleSheet);
    setWidgetResizable(true);
    setAlignment(Qt::AlignTop);

    clear();
}

void SegmentPanel::loadIntervals(const QVector<QPair<qint64, qint64> >& intervals)
{
    setAvailable(false);
    m_subrip.setIntervals(intervals);
    loadSubrip();
}

void SegmentPanel::loadSubrip()
{
    setAvailable(false);

    if (widget()) widget()->deleteLater();

    setWidget(new QWidget);
    QVBoxLayout* vbox = new QVBoxLayout(widget());

    setUpdatesEnabled(false);
    for (int i = 0; i < size(); ++i) {
        SegmentEdit* subEdit = new SegmentEdit(m_subrip.at(i));
        vbox->addWidget(subEdit);
        CONNECT(subEdit, &SegmentEdit::startAbRepeat, this, &SegmentPanel::emitStartAbRepeat);
        CONNECT(subEdit, &SegmentEdit::stopAbRepeat, this, &SegmentPanel::emitStopAbRepeat);
        CONNECT(subEdit, &SegmentEdit::focusChanged, this, &SegmentPanel::onFocusChange);
        CONNECT(subEdit, &SegmentEdit::setPlaybackPosition, this, &SegmentPanel::setPlaybackPosition);
    }
    if (size()) {
        vbox->addStretch(1);
        _setFocus(0);
        setAvailable(true);
        updatePlaybackPosition(m_playbackPosition);
    }
    setUpdatesEnabled(true);
}

void SegmentPanel::onAbRepeatStop()
{
    if (m_repeating) {
        m_repeating->toggleAbRepeat(false);
        m_repeating = nullptr;
    }
}

void SegmentPanel::updatePlaybackPosition(qint64 msec)
{
    m_playbackPosition = msec;
    if ( ! isAvailable()) return;

    const int index = seekIndex(msec);
    // LOG << index << msec;

    if (m_playing != index) {
        highlight(index);
    }
}

qint64 SegmentPanel::getBeginTime(int index)
{
    if (index >= 0 && index < size())
        return m_subrip.at(index)->begin;
    else if (index == size())
        return std::numeric_limits<qint64>::max();
    else
        Q_UNREACHABLE();
}

int SegmentPanel::tryIndex(qint64 msec, int index)
{
    const qint64 cur = getBeginTime(index);
    const qint64 next = getBeginTime(index+1);
    if (cur <= msec && next > msec)
        return 0;
    else if (cur > msec)
        return index ? -1 : 0;
    else
        return 1;
}

void SegmentPanel::clear()
{
    setAvailable(false);
    m_playing = None;
    m_repeating = nullptr;
    m_focused = nullptr;
    m_playbackPosition = 0;
    m_subrip.clear();

    loadSubrip();
}

void SegmentPanel::open(const QString& fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream ts(&file);
        ts >> m_subrip;
        file.close();

        loadSubrip();
    } else {
        qCritical() << "Unable to open" << fileName;
    }
}

void SegmentPanel::saveAs(const QString& fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream ts(&file);
        ts << m_subrip;
        ts.flush();
        file.close();
    } else {
        qCritical() << "Unable to write to" << fileName;
    }
}

void SegmentPanel::setAvailable(bool available)
{
    if (m_available != available)
        emit availableChanged((m_available = available));
}

int SegmentPanel::seekIndex(qint64 msec, int guess)
{
    int ret;
    do {
        ret = tryIndex(msec, guess);
        guess += ret;
    } while (ret);
    return guess;
}

int SegmentPanel::seekIndex(qint64 msec)
{
    return seekIndex(msec, m_playing == None ? 0 : m_playing);
}

void SegmentPanel::highlight(int index)
{
    if (m_playing != None)
        seekChild(m_playing)->toggleBackgroundImage(false);
    seekChild(index)->toggleBackgroundImage(true);
    m_playing = index;
}

SegmentEdit* SegmentPanel::seekChild(int index)
{
    Q_ASSERT(index >= 0 && index < size());
    return static_cast<SegmentEdit*>(widget()->layout()->itemAt(index)->widget());
}

int SegmentPanel::seekIndex(SegmentEdit* pointer)
{
    if ( ! pointer)
        return None;
    for (int i = 0; i < size(); ++i)
        if (seekChild(i) == pointer)
            return i;
    return None;
}

void SegmentPanel::shiftFocus(int shift)
{
    if ( ! isAvailable()) return;
    if ( ! m_focused) return;
    const int index = seekIndex(m_focused);
    if (index == None) return;
    _setFocus(bounded(index + shift));
}

void SegmentPanel::focusPrevious()
{
    shiftFocus(-1);
}

void SegmentPanel::focusNext()
{
    shiftFocus(1);
}

void SegmentPanel::showCurrent()
{
    if (m_playing == None) return;
    _setFocus(m_playing);
}

void SegmentPanel::shiftPlaybackPosition(int shift)
{
    if ( ! isAvailable()) return;
    if (m_playing == None) return;

    const int index = bounded(m_playing + shift);
    if (m_repeating)
        _emitStartAbRepeat(index);
    else
        emit setPlaybackPosition(getBeginTime(index));
    _setFocus(index);
}

void SegmentPanel::playPrevious()
{
    shiftPlaybackPosition(-1);
}

void SegmentPanel::playNext()
{
    shiftPlaybackPosition(1);
}

void SegmentPanel::_emitStartAbRepeat(int index)
{
    Q_ASSERT(index >= 0 && index < size());
    const qint64 msecA = m_subrip.at(index)->begin;
    const qint64 msecB = m_subrip.at(index)->end;

    if (m_repeating) m_repeating->toggleAbRepeat(false);
    m_repeating = seekChild(index);
    m_repeating->toggleAbRepeat(true);

    emit startAbRepeat(msecA, msecB);
}

void SegmentPanel::emitStartAbRepeat(qint64 msecA, qint64 msecB)
{
    if (m_repeating) m_repeating->toggleAbRepeat(false);
    m_repeating = static_cast<SegmentEdit*>(sender());
    emit startAbRepeat(msecA, msecB);
}

void SegmentPanel::emitStopAbRepeat()
{
    if (m_repeating) m_repeating->toggleAbRepeat(false);
    m_repeating = nullptr;
    emit stopAbRepeat();
}

int SegmentPanel::bounded(int index)
{
    return qBound(0, index, size() - 1);
}

void SegmentPanel::onFocusChange(bool focused)
{
    m_focused = focused ? static_cast<SegmentEdit*>(sender()) : nullptr;
}

void SegmentPanel::_setFocus(int index)
{
    Q_ASSERT(index >= 0 && index < size());
    SegmentEdit* child = seekChild(index);
    child->setFocus();
    ensureWidgetVisible(child, 0, height() / 2);
}

void SegmentPanel::toggleAbRepeat()
{
    _toggleAbRepeat( ! m_repeating);
}

void SegmentPanel::_toggleAbRepeat(bool repeat)
{
    if (m_playing == None) return;

    if (repeat) {
        _emitStartAbRepeat(m_playing);
    } else {
        emitStopAbRepeat();
    }
}

void SegmentPanel::playFocusedSegment()
{
    if ( ! m_focused) return;

    m_focused->emitSetPlaybackPosition();
}

/*
void SegmentPanel::resizeEvent(QResizeEvent* event)
{
    LOG << event;
    super::resizeEvent(event);
}
*/

#undef  LOG

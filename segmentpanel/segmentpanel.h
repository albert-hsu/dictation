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

#ifndef __SEGMENTPANEL_H__
#define __SEGMENTPANEL_H__

#include "subrip.h"
#include "segmentedit.h"

#include <QScrollArea>
#include <QString>
#include <QVector>
#include <QPair>
#include <QResizeEvent>

class SegmentPanel: public QScrollArea {
Q_OBJECT
    typedef QScrollArea super;
    const int None = -1;
public:
    explicit SegmentPanel(QWidget* parent);

    void clear();
    void open(const QString& fileName);
    void saveAs(const QString& fileName);
    bool isAvailable() {return m_available;}
    int size() {return m_subrip.size();}
signals:
    void startAbRepeat(qint64 begin, qint64 end);
    void stopAbRepeat();
    void setPlaybackPosition(qint64 position);
    void availableChanged(bool available);
public slots:
    void loadIntervals(const QVector<QPair<qint64, qint64> >& intervals);
    void onAbRepeatStop();
    void updatePlaybackPosition(qint64 msec);
    void focusPrevious();
    void focusNext();
    void showCurrent();
    void playPrevious();
    void playNext();
    void toggleAbRepeat();
    void playFocusedSegment();
private slots:
    void emitStartAbRepeat(qint64 msecA, qint64 msecB);
    void emitStopAbRepeat();
    void onFocusChange(bool focused);
protected:
    // void resizeEvent(QResizeEvent*) Q_DECL_OVERRIDE;
private:
    SubRip m_subrip;

    void loadSubrip();
    void setAvailable(bool available);
    void highlight(int index);
    qint64 getBeginTime(int index);
    int bounded(int index);
    int tryIndex(qint64 msec, int index);
    int seekIndex(qint64 msec, int guess);
    int seekIndex(qint64 msec);
    int seekIndex(SegmentEdit* pointer);
    SegmentEdit* seekChild(int index);
    void _setFocus(int index);
    void shiftFocus(int shift);
    void shiftPlaybackPosition(int shift);
    void _emitStartAbRepeat(int index);
    void _toggleAbRepeat(bool repeat);

    bool m_available = false;
    int m_playing;
    SegmentEdit* m_repeating;
    SegmentEdit* m_focused;
    int m_playbackPosition;
};

#endif

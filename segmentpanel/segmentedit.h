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

#ifndef __SEGMENTEDIT_H__
#define __SEGMENTEDIT_H__

#include "subrip.h"
#include "textedit.h"
#include "customwidgets/button.h"

#include <QWidget>
#include <QPixmap>
#include <QString>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QLabel>
#include <QTimer>

class SegmentEdit: public QWidget {
Q_OBJECT
    typedef QWidget super;
public:
    explicit SegmentEdit(Subtitle* subtitle);

    void toggleAbRepeat(bool repeat);
    void toggleBackgroundImage(bool display);
    void emitSetPlaybackPosition();
signals:
    void startAbRepeat(qint64 msecA, qint64 msecB);
    void stopAbRepeat();
    void focusChanged(bool focused);
    void setPlaybackPosition(qint64 msec);
protected:
    void mouseDoubleClickEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
    // void resizeEvent(QResizeEvent*) Q_DECL_OVERRIDE;
private slots:
    void _toggleAbRepeat(bool repeat);
    void restartTimer();
    void updateText();
private:
    Subtitle* m_sub;
    QLabel* m_time;
    TextEdit* m_text;
    Button* m_repeatButton;
    QTimer* m_timer;

    bool m_displayBackgroundImage;
    QPixmap m_backgroundImage;

    void loadContents();
    QString toTimecode(qint64 msec);
};

#endif

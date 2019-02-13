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
#ifdef  SEGMENTEDIT_DEBUG
#define LOG qDebug()<<Q_FUNC_INFO<<"$"
#else
#define LOG NullDebug()
#endif
#endif

#include "segmentedit.h"
#include "utils/debug.h"
#include "utils/color.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>

SegmentEdit::SegmentEdit(Subtitle* subtitle): m_sub(subtitle)
{
    m_time = new QLabel(this);

    m_text = new TextEdit(this);
    m_text->setFixedSize(QSize(350, 70));
    m_text->setStatusTip(tr("Write the words you hear in the box."));
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
    m_text->setStyleSheet(styleSheet);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);

    m_repeatButton = new Button(QPixmap(":/repeat.png"), this);
    m_repeatButton->setFocusProxy(m_text);
    m_repeatButton->setFocusPolicy(Qt::ClickFocus);
    m_repeatButton->setCheckedIcon(QPixmap(":/stop-repeat.png"));
    m_repeatButton->setCheckable(true);
    m_repeatButton->setIconSize(QSize(25, 25));

    m_repeatButton->setColor(Color::ButtonHovered,
        IconRole::Hovered);
    m_repeatButton->setColor(Color::ButtonPressed,
        IconRole::Hovered | IconRole::Pressed);
    m_repeatButton->setColor(Color::ButtonHovered,
        IconRole::Checked | IconRole::Hovered);
    m_repeatButton->setColor(Color::ButtonPressed,
        IconRole::Checked | IconRole::Hovered | IconRole::Pressed);

    setFocusProxy(m_text);
    setFocusPolicy(Qt::ClickFocus);

    m_backgroundImage = Color::fillColor(QPixmap(":/dialog.png"), Color::Dark);

    CONNECT(m_repeatButton, &Button::clicked, this, &SegmentEdit::_toggleAbRepeat);
    CONNECT(m_text, &TextEdit::focusChanged, this, &SegmentEdit::focusChanged);
    CONNECT(m_text, &TextEdit::textChanged, this, &SegmentEdit::restartTimer);
    CONNECT(m_timer, &QTimer::timeout, this, &SegmentEdit::updateText);

    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->addWidget(m_time);
    hbox->addStretch(1);
    hbox->addWidget(m_repeatButton);
    QVBoxLayout* vbox = new QVBoxLayout(this);
    vbox->addLayout(hbox);
    vbox->addWidget(m_text);
    vbox->setAlignment(m_text, Qt::AlignHCenter);
    vbox->setContentsMargins(50, 10, 25, 10);

    setFixedSize(QSize(448, 121));

    loadContents();
    toggleAbRepeat(false);
    toggleBackgroundImage(false);
}

void SegmentEdit::loadContents()
{
    m_time->setText(toTimecode(m_sub->begin) + QStringLiteral(" \u2014 ") + toTimecode(m_sub->end));
    m_text->setPlainText(m_sub->text);
}

void SegmentEdit::_toggleAbRepeat(bool repeat)
{
    LOG << "repeat" << repeat;
    repeat ?
        emit startAbRepeat(m_sub->begin, m_sub->end) :
        emit stopAbRepeat();
    toggleAbRepeat(repeat);
}

void SegmentEdit::toggleAbRepeat(bool repeat)
{
    // LOG << "repeat" << repeat;
    m_repeatButton->setChecked(repeat);
    m_repeatButton->setStatusTip(repeat ? tr("Stop repeating.") : tr("Repeat."));
}

void SegmentEdit::mouseDoubleClickEvent(QMouseEvent* event)
{
    super::mouseDoubleClickEvent(event);
    if (event->button() == Qt::LeftButton) {
        emitSetPlaybackPosition();
    }
}

void SegmentEdit::emitSetPlaybackPosition()
{
    emit stopAbRepeat();
    // FIXME: (Qt 5.11) On Linux, The playback position cannot be set if a
    //  QMediaPlayer has not "actually" started playing.
    //  Note that, in this case, QMediaPlay::play() only "schedules" a play.
    //  This issue does not occur on Windows if Media Foundation is used as
    //  the multimedia backend.
    LOG << m_sub->begin;
    emit setPlaybackPosition(m_sub->begin);
}

QString SegmentEdit::toTimecode(qint64 msec)
{
    Q_ASSERT(msec >= 0 && msec < 86400000);

    const qint64 hour = msec / 3600000;
    const qint64 min = msec / 60000 % 60;
    const qint64 sec = msec / 1000 % 60;
    const qint64 msec_ = msec % 1000;

    const QChar zero('0');

    if (Q_UNLIKELY(hour))
        return QStringLiteral("%1:%2:%3.%4")
            .arg(hour, 2, 10, zero)
            .arg(min, 2, 10, zero)
            .arg(sec, 2, 10, zero)
            .arg(msec_, 3, 10, zero);
   else
        return QStringLiteral("%1:%2.%3")
            .arg(min, 2, 10, zero)
            .arg(sec, 2, 10, zero)
            .arg(msec_, 3, 10, zero);
}

void SegmentEdit::restartTimer()
{
    m_timer->start(1000);
}

void SegmentEdit::updateText()
{
    m_sub->text = m_text->toPlainText();
}

void SegmentEdit::toggleBackgroundImage(bool display)
{
    m_displayBackgroundImage = display;

    const int fontColor = display ? Color::Light : Color::Dark;
    QPalette P = palette();
    P.setColor(QPalette::WindowText, fontColor);
    setPalette(P);

    m_repeatButton->setColor(display ? Color::Light : Color::Dark,
        IconRole::Initial);
    m_repeatButton->setColor(display ? Color::Light : Color::Dark,
        IconRole::Checked);
}

/*
void SegmentEdit::resizeEvent(QResizeEvent* event)
{
    LOG << event;
    super::resizeEvent(event);
}
*/

// NOTE: A workaround to make the background image not repeated.
//  Background image repeaing can only be stopped by Qt Style Sheet,
//  and unfortunately QSS cannot be used in conjunction with QPalette,
//  thus we paint the image with paintEvent()
void SegmentEdit::paintEvent(QPaintEvent* event)
{
    if (m_displayBackgroundImage) {
        QPainter P(this);
        P.drawPixmap(QPoint(0, 0), m_backgroundImage);
    }
    super::paintEvent(event);
}

#undef  LOG

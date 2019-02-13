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
#ifdef  CONTROLBUTTONSET_DEBUG
#define LOG qDebug()<<Q_FUNC_INFO<<"$"
#else
#define LOG NullDebug()
#endif
#endif

#include "controlbuttonset.h"
#include "utils/debug.h"
#include "utils/color.h"

#include <QHBoxLayout>

ControlButtonSet::ControlButtonSet(QWidget* parent):
    QWidget(parent)
{
    m_playButton = new Button(QPixmap(":/play.png"), this);
    setUpButton(m_playButton);
    m_pauseIcon = m_playButton->addIcon(QPixmap(":/pause.png"));
    setState(AudioPlayer::StoppedState);
    CONNECT(m_playButton, &Button::clicked, this, &ControlButtonSet::playClicked);

    m_previousButton = new Button(QPixmap(":/previous.png"), this);
    m_previousButton->setStatusTip(tr("Skip backward. (Ctrl+B)"));
    setUpButton(m_previousButton);
    CONNECT(m_previousButton, &Button::clicked, this, &ControlButtonSet::previous);

    m_nextButton = new Button(QPixmap(":/next.png"), this);
    m_nextButton->setStatusTip(tr("Skip forward. (Ctrl+F)"));
    setUpButton(m_nextButton);
    CONNECT(m_nextButton, &Button::clicked, this, &ControlButtonSet::next);

    QHBoxLayout* hbox = new QHBoxLayout(this);
    hbox->addWidget(m_previousButton);
    hbox->addWidget(m_playButton);
    hbox->addWidget(m_nextButton);
    hbox->setContentsMargins(0, 0, 0, 0);

    setContentsMargins(0, 0, 0, 0);
}

void ControlButtonSet::setState(AudioPlayer::State state)
{
    LOG << state;
    m_state = state;

    switch(state) {
    case AudioPlayer::StoppedState:
    case AudioPlayer::PausedState:
        m_playButton->swapIcon(Button::OriginalIcon);
        m_playButton->setStatusTip(tr("Play. (Ctrl+P)"));
        break;
    case AudioPlayer::PlayingState:
        m_playButton->swapIcon(m_pauseIcon);
        m_playButton->setStatusTip(tr("Pause. (Ctrl+P)"));
        break;
    default:
        Q_UNREACHABLE();
    }
}

void ControlButtonSet::playClicked()
{
    switch(m_state) {
    case AudioPlayer::StoppedState:
    case AudioPlayer::PausedState:
        emit play();
        break;
    case AudioPlayer::PlayingState:
        emit pause();
        break;
    default:
        Q_UNREACHABLE();
    }
}

void ControlButtonSet::setUpButton(Button* button)
{
    button->setIconSize(QSize(25, 25));

    button->setColor(Color::Dark,
        IconRole::Initial);
    button->setColor(Color::Carrot,
        IconRole::Hovered);
    button->setColor(Color::Pumpkin,
        IconRole::Hovered | IconRole::Pressed);
}

#undef  LOG

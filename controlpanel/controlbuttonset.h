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

#ifndef __CONTROLBUTTONSET_H__
#define __CONTROLBUTTONSET_H__

#include "audioplayer/audioplayer.h"
#include "customwidgets/button.h"

#include <QWidget>

class ControlButtonSet: public QWidget
{
Q_OBJECT
public:
    explicit ControlButtonSet(QWidget* parent);
signals:
    void play();
    void pause();
    void previous();
    void next();
public slots:
    void setState(AudioPlayer::State state);
private:
    // NOTE: Display play & pause icons alternatively on click
    Button* m_playButton;
    Button* m_previousButton;
    Button* m_nextButton;

    AudioPlayer::State m_state;
    int m_pauseIcon;

    void playClicked();
    void setUpButton(Button* button);
};

#endif

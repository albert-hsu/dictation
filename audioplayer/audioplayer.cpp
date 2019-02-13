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
#ifdef  AUDIOPLAYER_DEBUG
#define LOG qDebug()<<Q_FUNC_INFO<<"$"
#else
#define LOG NullDebug()
#endif
#endif

#include "audioplayer.h"
#include "utils/debug.h"

#include <QTimer>

AudioPlayer::AudioPlayer(QWidget* parent): QMediaPlayer(parent)
{
    setNotifyInterval(150);
    CONNECT(this, &QMediaPlayer::positionChanged, this, &AudioPlayer::abRepeat);
    CONNECT(this, &QMediaPlayer::stateChanged, this, &AudioPlayer::onStateChange);
}

void AudioPlayer::startAbRepeat(qint64 msecA, qint64 msecB)
{
    LOG << msecA << msecB;

    // NOTE: May have some race conditions
    m_isRepeating = false;
    m_a = msecA;
    m_b = msecB;
    m_isRepeating = true;

    if (state() != PlayingState) play();
}

void AudioPlayer::stopAbRepeat()
{
    m_isRepeating = false;
}

void AudioPlayer::_stopAbRepeat()
{
    stopAbRepeat();
    emit abRepeatStopped();
}

void AudioPlayer::setPosition(qint64 msec)
{
    _stopAbRepeat();
    QMediaPlayer::setPosition(msec);
}

void AudioPlayer::togglePlayPause()
{
    state() != PlayingState ? play() : pause();
}

void AudioPlayer::moveTo(qint64 msec)
{
    const int delay = 800;
    bool b;

    QMediaPlayer::setPosition(msec);
    b = blockSignals(true);
    pause();
    blockSignals(b);

    // NOTE: QTimer::singleShot() creates a delay without blocking the thread
    QTimer::singleShot(delay, this, [=]() {
        bool b = blockSignals(true);
        play();
        blockSignals(b);
    });
}

void AudioPlayer::abRepeat(qint64 msec)
{
    if (m_isRepeating && (msec > m_b || msec < m_a)) {
#ifdef Q_OS_WIN
        // NOTE: (Qt 5.11) A workaround to play audio correctly on Windows if
        //  Media Foundation is used as the multimedia backend. Here,
        //  QTimer::singleShot() creates a delay without blocking the thread
        QTimer::singleShot(300, this, [=]() {
            moveTo(m_a);
        });
#else
        moveTo(m_a);
#endif
    }
}

void AudioPlayer::setSourceFileName(const QString& fileName)
{
    setMedia(QUrl::fromLocalFile(fileName));
}

void AudioPlayer::clear()
{
    _stopAbRepeat();
    setMedia(QMediaContent());
}

void AudioPlayer::onStateChange(State state)
{
    if (state != PlayingState)
        _stopAbRepeat();
}

#undef LOG

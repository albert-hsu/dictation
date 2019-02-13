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
#ifdef  SPEECHSEGMENTER_DEBUG
#define LOG qDebug()<<Q_FUNC_INFO<<"$"
#else
#define LOG NullDebug()
#endif
#endif

#include "speechsegmenter.h"
#include "utils/debug.h"

#include <algorithm>

SpeechSegmenter::SpeechSegmenter(QObject* parent): QThread(parent)
{
    m_format.setChannelCount(1);
    m_format.setSampleSize(16);
    m_format.setSampleRate(44100);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::SignedInt);

    m_decoder = new QAudioDecoder(this);
    m_decoder->setAudioFormat(m_format);

    CONNECT_5(m_decoder, &QAudioDecoder::bufferReady, this, &SpeechSegmenter::collectSegments, Qt::DirectConnection);
    CONNECT(m_decoder, static_cast<void (QAudioDecoder::*)(QAudioDecoder::Error)>(&QAudioDecoder::error), this, &SpeechSegmenter::emitError);
    CONNECT(m_decoder, &QAudioDecoder::finished, this, &SpeechSegmenter::mergeSegments);
}

void SpeechSegmenter::setSourceFileName(const QString &fileName)
{
    m_decoder->setSourceFilename(fileName);
}

void SpeechSegmenter::run()
{
    m_rightClosed = true;
    m_rightEnd = -1e10;
    m_frameCount = 0;
    m_queue.clear();

    m_decoder->start();
}

void SpeechSegmenter::collectSegments()
{
    const qint16 Threashold = 500;
    const qint64 FrameDifference = 10000;

    const QAudioBuffer buffer = m_decoder->read();
    if ( ! buffer.isValid()) {
        m_decoder->stop();
        emit error(SpeechSegmenter::BufferError);
        return;
    }
    const qint16* data = buffer.constData<qint16>();

    for (int i = 0; i < buffer.frameCount(); i += m_format.channelCount()) {
        const qint16 amplitude = qAbs(*(data + i));
        if (m_rightClosed && amplitude > Threashold) {
            const qint64 frameIndex = m_frameCount + i;
            if (frameIndex - m_rightEnd > FrameDifference) {
                m_queue.append(m_rightEnd);
                m_queue.append(frameIndex);
            }
            m_rightClosed = false;
        } else if ( ! m_rightClosed && amplitude <= Threashold) {
            m_rightEnd = m_frameCount + i;
            m_rightClosed = true;
        }
    }
    m_frameCount += buffer.frameCount();
}

void SpeechSegmenter::mergeSegments()
{
    qint64 duration = toMsec(m_frameCount);
    if (duration >= 3600000) {
        emit error(SpeechSegmenter::DurationTooLongError);
        return;
    }
    emit durationChanged(duration);

    if (m_queue.size() == 0) {
        emit error(SpeechSegmenter::SilentTrackError);
        return;
    }

    m_queue.removeFirst();
    if (m_queue.size() % 2) {
        LOG << "the last interval is not closed";
        m_queue.append(m_rightEnd);
    }
    Q_ASSERT(m_queue.size() % 2 == 0);
    LOG << "number of unmerged segments =" << m_queue.size() / 2;

    const int percentile = 45;
    const int n = m_queue.size() / 2 * percentile / 100;

    // A collection of the length of silent intervals in frame difference
    auto S = new QVector<qint64>();
    S->reserve(m_queue.size() / 2);
    qint64 prev = 0;
    for (auto it = m_queue.begin(); it != m_queue.end(); ++it) {
        S->append(*it - prev);
        prev = *++it;
    }
    std::sort(S->begin(), S->end(), std::greater<qint64>());
    const qint64 nthLargest = S->at(qMin(n, S->size()-1));
    delete S;

    QVector<Interval> intervals;
    qint64 begin = m_queue.first(), end = *(m_queue.begin()+1);
    auto last = --m_queue.end();

    if (m_queue.size() == 2) {
        intervals.append(qMakePair(toMsec(begin), toMsec(end)));
    } else {
        for (auto it = m_queue.begin()+2; it != m_queue.end(); ++it) {
            if (*it - end > nthLargest) {
                intervals.append(qMakePair(toMsec(begin), toMsec(end)));
                begin = *it;
            }
            end = *++it;
            if (it == last)
                intervals.append(qMakePair(toMsec(begin), toMsec(end)));
        }
    }
    emit segmentsChanged(intervals);
}

qint64 SpeechSegmenter::toMsec(qint64 frameIndex)
{
    return frameIndex * 1000 / m_format.sampleRate();
}

void SpeechSegmenter::emitError(QAudioDecoder::Error e)
{
    emit error(static_cast<SpeechSegmenter::Error>(e));
}

#undef  LOG

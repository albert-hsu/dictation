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

#ifndef __SPEECHSEGMENTER_H__
#define __SPEECHSEGMENTER_H__

#include <QAudioDecoder>
#include <QThread>
#include <QLinkedList>

class SpeechSegmenter: public QThread
{
Q_OBJECT
    typedef QPair<qint64, qint64> Interval;
public:
    explicit SpeechSegmenter(QObject* parent = 0);

    enum Error {
        NoError = QAudioDecoder::NoError,
        ResourceError = QAudioDecoder::ResourceError,
        FormatError = QAudioDecoder::FormatError,
        AccessDeniedError = QAudioDecoder::AccessDeniedError,
        ServiceMissingError = QAudioDecoder::ServiceMissingError,

        BufferError,
        SilentTrackError,
        DurationTooLongError
    };

    void setSourceFileName(const QString &fileName);
    void run() Q_DECL_OVERRIDE;
signals:
    void error(SpeechSegmenter::Error);
    void durationChanged(qint64 msec);
    void segmentsChanged(QVector<Interval> intervals);
private slots:
    void collectSegments();
    void mergeSegments();
    void emitError(QAudioDecoder::Error e);
private:
    QAudioFormat m_format;
    QAudioDecoder* m_decoder;

    qint64 m_frameCount;
    bool m_rightClosed;
    qint64 m_rightEnd;
    QLinkedList<qint64> m_queue;

    qint64 toMsec(qint64 frameIndex);
};

#endif

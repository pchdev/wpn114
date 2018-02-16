#ifndef QSINE_H
#define QSINE_H

#include <QObject>
#include <QAudioOutput>
#include "audiobackend.h"

#define WT_SIZE 16384

class SinOsc : public AudioObject
{
    Q_OBJECT
    Q_PROPERTY ( qreal frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)

public:

    SinOsc();
    virtual float** process(const quint16 nsamples) override;

    qreal frequency() const;
    void setFrequency(const qreal);

signals:
    void frequencyChanged();

private:
    quint16 m_pos;
    qreal m_frequency;
    float m_wavetable[WT_SIZE];
};

#endif // QSINE_H

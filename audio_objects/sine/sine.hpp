#ifndef QSINE_H
#define QSINE_H

#include <QObject>
#include <src/audio/audio.hpp>

#define WT_SIZE 16384

class SinOsc : public StreamNode
{
    Q_OBJECT
    Q_PROPERTY ( qreal frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged )

public:

    SinOsc();
    virtual float** userProcess(float** buf, qint64 le) override;
    virtual void userInitialize(qint64 le) override { Q_UNUSED(le); }

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

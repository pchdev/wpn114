#ifndef MASTERLIMITER_HPP
#define MASTERLIMITER_HPP

#include <src/audio/audio.hpp>

class MasterLimiter : public StreamNode
{
    Q_OBJECT

    Q_PROPERTY  ( qreal threshold READ threshold WRITE setThreshold )
    Q_PROPERTY  ( qreal release READ release WRITE setRelease )
    Q_PROPERTY  ( qreal limit READ limit WRITE setLimit )

    public:
    MasterLimiter();

    void initialize(qint64 nsamples) override;
    float** process(float** in, qint64 nsamples) override;

    qreal threshold() const { return m_threshold; }
    qreal release() const { return m_release; }
    qreal limit() const { return m_limit; }

    void setThreshold(qreal threshold);
    void setRelease(qreal release);
    void setLimit(qreal limit);

    private:
    qreal m_threshold  = -0.1;
    qreal m_release    = 200;
    qreal m_limit      = -0.1;

    float holdtime = 0.f;
    float r1timer = 0.f;
    float r2timer = 0.f;
    float max1block = 0.f;
    float max2block = 0.f;
    float gain = 0.f;
};

#endif // MASTERLIMITER_HPP

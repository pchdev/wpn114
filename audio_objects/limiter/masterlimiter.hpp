#ifndef MASTERLIMITER_HPP
#define MASTERLIMITER_HPP

#include <src/audio/audio.hpp>

class MasterLimiter : public StreamNode
{
    Q_OBJECT

    Q_PROPERTY  ( qreal threshold READ threshold WRITE setThreshold )
    Q_PROPERTY  ( qreal lookAhead READ lookAhead WRITE setLookAhead )
    Q_PROPERTY  ( qreal attack READ attack WRITE setAttack )
    Q_PROPERTY  ( qreal hold READ hold WRITE setHold )
    Q_PROPERTY  ( qreal release READ release WRITE setRelease )
    Q_PROPERTY  ( qreal limit READ limit WRITE setLimit )

    public:
    MasterLimiter();

    void initialize(qint64 nsamples) override;
    float** process(float** in, qint64 nsamples) override;

    qreal threshold() const { return m_threshold; }
    qreal lookAhead() const { return m_lookahead; }
    qreal attack() const { return m_attack; }
    qreal hold() const { return m_hold; }
    qreal release() const { return m_release; }
    qreal limit() const { return m_limit; }

    void setThreshold(qreal threshold);
    void setLookAhead(qreal lookahead);
    void setAttack(qreal attack);
    void setHold(qreal hold);
    void setRelease(qreal release);
    void setLimit(qreal limit);

    private:
    qreal m_threshold  = -0.1;
    qreal m_lookahead  = 200;
    qreal m_attack     = 100;
    qreal m_hold       = 0;
    qreal m_release    = 250;
    qreal m_limit      = -0.1;
};

#endif // MASTERLIMITER_HPP

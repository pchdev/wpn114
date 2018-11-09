#include "masterlimiter.hpp"
#include <math.h>

MasterLimiter::MasterLimiter()
{

}

void MasterLimiter::setThreshold(qreal threshold)
{
    m_threshold = threshold;
}

void MasterLimiter::setLookAhead(qreal lookahead)
{
    m_lookahead = lookahead;
}

void MasterLimiter::setAttack(qreal attack)
{
    m_attack = attack;
}

void MasterLimiter::setHold(qreal hold)
{
    m_hold = hold;
}

void MasterLimiter::setRelease(qreal release)
{
    m_release = release;
}

void MasterLimiter::setLimit(qreal limit)
{
    m_limit = limit;
}

void MasterLimiter::initialize(qint64 nsamples)
{

}

float** MasterLimiter::process(float** in, qint64 nsamples)
{

}

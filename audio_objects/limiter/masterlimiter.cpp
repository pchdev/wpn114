#include "masterlimiter.hpp"
#include <math.h>

MasterLimiter::MasterLimiter()
{
    SETTYPE( StreamType::Effect );
}

void MasterLimiter::setThreshold(qreal threshold)
{
    m_threshold = threshold;
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
    holdtime = m_stream_properties.sample_rate/128.f;
    r1timer = 0;
    r2timer = holdtime/2.f;
}

float** MasterLimiter::process(float** in, qint64 nsamples)
{
    auto out        = m_out;
    auto nout       = m_num_outputs;

    float thresh    = pow( 10, m_threshold/20.f );
    float ceiling   = pow( 10, m_limit/20.f );
    float volume    = ceiling/thresh;
    float release   = m_release/1000.f;

    float r = exp(-3/( SAMPLERATE*qMax( release , 0.05f) ));

    for ( qint64 s = 0; s < nsamples; ++s )
    {
        for ( quint16 ch = 0; ch < nout; ++ch )
        {
            float maxspl = fabs( in[ch][s] );

            if ( r1timer++ > holdtime )
            {
                r1timer     = 0;
                max1block   = 0;
            }

            if ( r2timer++ > holdtime )
            {
                r2timer     = 0;
                max2block   = 0;
            }

            max1block  = qMax ( max1block, maxspl );
            float envt = qMax ( max1block, max2block );

            float env = env < envt ? envt : envt+r*(env-envt);

            if ( env > thresh )
                 gain = thresh/env*volume;
            else gain = volume;

            out[ch][s] = in[ch][s]*gain;
        }
    }

    return out;
}

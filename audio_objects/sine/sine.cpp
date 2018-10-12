#include "sine.hpp"
#include <math.h>

SinOsc::SinOsc() : StreamNode(), m_frequency(440.f), m_pos(0)
{
    SETN_OUT    ( 1 );

    for( quint16 i = 0; i < WT_SIZE; ++i )
        m_wavetable[i] = sin ((float) i/WT_SIZE*M_PI*2 );
}

float** SinOsc::process(float** buf, qint64 nsamples)
{
    float level         = m_level;
    quint16 pos         = m_pos;
    qreal frequency     = m_frequency;
    quint16 incr        = frequency/SAMPLERATE * WT_SIZE;
    float** out         = m_out;

    for ( quint16 s = 0; s < nsamples; ++s )
    {
        for( quint16 ch = 0; ch < m_num_outputs; ++ ch)
            out[ch][s] = m_wavetable[pos] * level;

        pos += incr;

        if ( pos >= WT_SIZE )
             pos -= WT_SIZE;
    }

    m_pos = pos;
    return out;
}

qreal SinOsc::frequency() const
{
    return m_frequency;
}

void SinOsc::setFrequency(const qreal frequency)
{
    m_frequency = frequency;
}


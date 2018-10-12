#include "sharpen.hpp"
#include <algorithm>
#include <cmath>

Sharpen::Sharpen() : m_distortion(0)
{
    SETN_IN     ( 2 );
    SETN_OUT    ( 2 );
}

void Sharpen::initialize(qint64 dist)
{

}

float** Sharpen::process(float** in, qint64 nsamples)
{
    auto dist   = std::min(m_distortion/100.f, 0.999);
    auto coeff  = 2.f*dist/(1.f-dist);
    auto nout   = m_num_outputs;
    auto out    = m_out;

    for ( quint16 s = 0; s < nsamples; ++s)
    {
        for ( quint16 ch = 0; ch < nout; ++ch )
        {
            float m     = std::min(std::max(in[ch][s], -1.f),1.f);
            out[ch][s]  = (1.f+coeff)*m/(1.f+coeff*fabs(m));
        }
    }

    return out;
}

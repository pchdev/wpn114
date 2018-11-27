#include "filter.hpp"
#include "math.h"

Filter::Filter()
{

}

void Filter::setHpf(qreal hpf)
{
    m_hpf = hpf;
}

void Filter::setLpf(qreal lpf)
{
    m_lpf = lpf;
}

void Filter::initialize(qint64)
{
    b_hpf = false;
    b_lpf = false;

    for ( quint16 i = 0; i < m_num_outputs; ++i )
        m_coeffs << chcoeff{ };
}

float** Filter::process(float** in, qint64 nsamples)
{
    auto nin = m_num_inputs;
    auto nout = m_num_outputs;
    auto out = m_out;

    freq1 = m_hpf;
    freq3 = m_lpf;
    b_hpf = !(m_hpf == 0.0);
    b_lpf = !(m_lpf == 22000.0);
    gain = pow(10, m_gain);

    a1 = 1;
    s1 = 1;
    q1 = 1 / sqrt((a1+1/a1)*(1/s1-1) +2);
    w01 = 2*M_PI * freq1/(float)SAMPLERATE;
    cosw01 = cos(w01);
    sinw01 = sin(w01);
    alpha1 = sinw01/(2*q1);

    b01 = (1+cosw01)/2;
    b11 = -(1+cosw01);
    b21 = b01;
    a01 = 1+alpha1;
    a11 = -2*cosw01;
    a21 = 1-alpha1;
    b01 /= a01;
    b11 /= a01;
    b21 /= a01;
    a11 /= a01;
    a21 /= a01;

    a3 = 1;
    s3 = 1;
    q3 = 1 / (sqrt((a3+1/a3)*(1/s3-1)+ 2));
    w03 = 2*M_PI*freq3/(float)SAMPLERATE;
    cosw03 = cos(w03);
    sinw03 = sin(w03);
    alpha3 = sinw03 / (2*q3);

    b03 = (1-cosw03)/2;
    b13 = (1-cosw03);
    b23 = b03;
    a03 = 1+alpha3;
    a13 = -2*cosw03;
    a23 = 1-alpha3;

    b03 /= a03;
    b13 /= a03;
    b23 /= a03;
    a13 /= a03;
    a23 /= a03;

    for ( quint16 ch = 0; ch < nout; ++ch )
    {
        auto& coeffs = m_coeffs[ch];

        for ( quint16 s = 0; s < nsamples; ++s )
        {
            float spl = in[ch][s];

            if ( b_hpf )
            {
                coeffs.ospl = spl;
                spl = b01*spl + b11*coeffs.x11 + b21*coeffs.x21 - a11*coeffs.y11 - a21*coeffs.y21;
                coeffs.x21 = coeffs.x11;
                coeffs.x11 = coeffs.ospl;
                coeffs.y21 = coeffs.y11;
                coeffs.y11 = spl;
            }

            if ( b_lpf )
            {
                coeffs.ospl = spl;
                spl = b03*spl + b13*coeffs.x13 + b23*coeffs.x23 - a13*coeffs.y13 - a23*coeffs.y23;
                coeffs.x23 = coeffs.x13;
                coeffs.x13 = coeffs.ospl;
                coeffs.y23 = coeffs.y13;
                coeffs.y13 = spl;
            }

            out[ch][s] = spl;
        }
    }

    return out;
}



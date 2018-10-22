#include "mangler.hpp"
#include <math.h>

Mangler::Mangler()
{
    SETN_IN     ( 2 );
    SETN_OUT    ( 2 );
}

void Mangler::initialize(qint64)
{
    float itm1 = 0, itm2 = 0, otm1 = 0, otm2 = 0;
    float dcshift = 1;

    float relgain = 0;
    float shaper_amt = 0.857;
    float shaper_amt_2 = 0.9;

    float lut_start = 128;
    float lut = lut_start;

}

float** Mangler::process(float**, qint64)
{

    float gate_amt          = m_gate/100.0;
    float gate_open_time    = (0.05 + (1.f-gate_amt)*0.3) * SAMPLERATE;
    float fade_point        = gate_open_time*0.5;
    float gate_threshold    = 0.15 + gate_amt * 0.25;
    float gate_leakage      = (gate_amt > 0.5) ? 0 : (1.f-gate_amt)*0.2;
    float bitdepth          = m_bitdepth;
    float resol             = pow(2, bitdepth-1.0);
    float invresl           = 1.0/resol;
    float target_per_sample = SAMPLERATE/m_bad_resampler;
    float gain              = pow(2, m_input_gain/6.0);
    float dry_gain          = pow(2, m_dry_out/6.0);
    float wet_gain          = pow(2, m_wet_out/6.0);

    int left_bit_slider     = (int) m_thermonuclear | 0;
    float mix               = m_thermonuclear-left_bit_slider;
    int right_bit_slider    = (mix > 0) ? left_bit_slider + 1 : left_bit_slider;

//    float bit_1             = lut_start + left_bit_slider*16;
//    float bit_2             = lut_start + right_bit_slider*16;




    return m_out;
}

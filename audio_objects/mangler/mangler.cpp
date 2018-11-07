#include "mangler.hpp"
#include <math.h>

Mangler::Mangler()
{
    SETN_IN     ( 2 );
    SETN_OUT    ( 2 );
}

void Mangler::initialize(qint64)
{    
    // initialize bit patterns

}

float** Mangler::process(float** in, qint64 nsamples)
{

    StreamNode::resetBuffer(m_out, m_num_outputs, nsamples);

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

    float bit_1    = lut_start + left_bit_slider*16;
    float bit_2    = lut_start + right_bit_slider*16;

    for ( int i = 0; i < 8-bitdepth; ++i ) // ?
    {
        bit_1 += 1;
        bit_2 += 1;
    }

    for ( int i = 0; i < bitdepth-8; ++i ) // ?
    {
        bit_1 -= 1;
        bit_2 -= 1;
    }

    float clear_mask_1 = 0, clear_mask_2 = 0;
    float xor_mask_1 = 0, xor_mask_2 = 0;

    for ( int i = 0; i < bitdepth; ++i )
    {
        // ?
    }

//    float post_bit_gain = relgain[left_bit_slider] * (1-mix) + relgain[right_bit_slider]*mix;

    // RC filter params (hi/lo)
    float LPF_c = pow( 0.5, 5.0-m_love/25 );
    float LPF_r = pow( 0.5, m_jive/40-0.6 );
    float HPF_c = pow( 0.5, 5.0-m_love/32 );
    float HPF_r = pow( 0.5, 3.0 - m_jive/40 );

    float LRC = LPF_r*LPF_c;
    float HRC = HPF_r*HPF_c;

    for ( quint16 ch = 0; ch < numOutputs(); ++ch )
    {
        for( qint64 s = 0; s < nsamples; ++s )
        {
            float dry = in[ch][s];
            per_sample =  0.9995 * per_sample + 0.0005 * target_per_sample;

            // deliberately broken resampler
            // (BAD DIGITAL)

            sample_csr += 1.f;
            if ( sample_csr < next_sample && m_jive < 33150 )
            {
                m_out[ch][s] = last_sample;
            }
            else
            {
                next_sample += per_sample;
                if ( m_jive == 33150 ) sample_csr = next_sample;

                float s0 = m_out[ch][s]*gain;

            }




        }
    }










    return m_out;
}

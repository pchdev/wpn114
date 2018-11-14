#include "mangler.hpp"
#include <math.h>
#include <qmath.h>

Mangler::Mangler()
{
    SETN_IN     ( 2 );
    SETN_OUT    ( 2 );
}

void Mangler::setBitPattern( quint16 index,
qint8 b1, qint8 b2, qint8 b3, qint8 b4, qint8 b5, qint8 b6, qint8 b7, qint8 b8 )
{
    qint8* ptr = &lut[ index ];
    *ptr++ = b1; *ptr++ = b2; *ptr++ = b3; *ptr++ = b4;
    *ptr++ = b5; *ptr++ = b6; *ptr++ = b6; *ptr++ = b8;
}

inline float fOR(float lhs, float rhs)
{
    return (float)((int) lhs | (int)rhs);
}

inline float fAND(float lhs, float rhs)
{
    return (float)((int)lhs & (int)rhs);
}

void Mangler::initialize(qint64)
{    
    // initialize bit patterns
    setBitPattern( 0, 1, 1, 1, 1, 1, 1, 1, 1 );
    setBitPattern( 8, 0, 1, 1, 1, 1, 1, 1, 1 );
    setBitPattern( 16, -1, 1, 1, 1, 1, 1, 1, 1 );
    setBitPattern( 24, 1, 0, 1, 1, 1, 1, 1, 1 );
    setBitPattern( 32, 1, -1, 1, 1, 1, 1, 1, 1 );
    setBitPattern( 40, 1, 0, 0, 1, 1, 1, 1, 1 );
    setBitPattern( 48, -1, -1, 1, 1, 1, 1, 1, 1 );
    setBitPattern( 56, 1, 1, 1, 1, -1, 1, 1, 1 );
    setBitPattern( 64, 1, 1, 1, 1, 1, 0, 1, 1 );
    setBitPattern( 72, 1, -1, 0, -1, 0, 1, -1, 0 );
    setBitPattern( 80, 1, -1, 1, -1, 1, 1, -1, -1 );
    setBitPattern( 88, 0, 0, 0, 1, 0, 0, 0, 0 );
    setBitPattern( 96, 1, 0, -1, 0, 0, 0, 0, 0 );
    setBitPattern( 104, 1, -1, 0, 0, 1, 1, 0, 0 );
    setBitPattern( 112, -1, 0, -1, 1, 1, 0, 0, 0 );
    setBitPattern( 120, -1, -1, 1, 1, 0, 0, 0, 0 );
    setBitPattern( 128, 1, 1, 1, 1, 0, 0, 0, 0 );

    relgain[ 0 ] = 1;
    relgain[ 1 ] = 1;
    relgain[ 2 ] = 1;
    relgain[ 3 ] = 1;
    relgain[ 4 ] = 1;
    relgain[ 5 ] = 1;
    relgain[ 6 ] = 1;
    relgain[ 7 ] = 0.4f;
    relgain[ 8 ] = 0.08f;
    relgain[ 9 ] = 0.5f;
    relgain[ 10 ] = 0.08f;
    relgain[ 11 ] = 1.5f;
    relgain[ 12 ] = 3;
    relgain[ 13 ] = 0.2f;
    relgain[ 14 ] = 2;
    relgain[ 15 ] = 1;
    relgain[ 16 ] = 1;
}

float** Mangler::process(float** in, qint64 nsamples)
{
    StreamNode::resetBuffer( m_out, m_num_outputs, nsamples );

/*
    slider1:  Input Gain (dB)
    slider2:  Dry Out (dB)
    slider3:  Wet Out (dB)
    slider4:  Bad Resampler (Hz)
    slider5:  { Off,On,On+Noisier } - Bitcrusher
    slider6:  Thermonuclear War
    slider7:  Bitdepth
    slider8:  Gate (%)
    slider13: Love (%)
    slider14: Jive (%)
    slider15: { No,Murky,Confused,Unpleasant }>Attitude
*/

    // @slider/block --------------------------------------------------------

    auto out                = m_out;
    float gate_amt          = m_gate/100;
    float gate_open_time    = (0.05f + (1.f-gate_amt)*0.3f) * SAMPLERATE;
    float fade_point        = gate_open_time*0.5f;
    float gate_threshold    = 0.15f + gate_amt * 0.25f;
    float gate_leakage      = (gate_amt > 0.5f) ? 0 : (1.f-gate_amt)*0.2f;
    float bitdepth          = m_bitdepth;
    float resol             = pow( 2, bitdepth-1.f );
    float invresl           = 1.f/resol;
    float target_per_sample = (float) SAMPLERATE/m_bad_resampler;
    float gain              = pow( 2, m_input_gain/6 );
    float dry_gain          = pow( 2, m_dry_out/6 );
    float wet_gain          = pow( 2, m_wet_out/6 );

    // LOOKUP -----------------------------------------------------------------------

    int left_bit_slider     = ( int ) m_thermonuclear | 0;
    float mix               = m_thermonuclear-left_bit_slider;
    int right_bit_slider    = ( mix > 0 ) ? left_bit_slider + 1 : left_bit_slider;

    quint16 bit_1 = left_bit_slider*16;
    quint16 bit_2 = right_bit_slider*16;

    if ( bitdepth < 8 )
    {
        bit_1 += 8-bitdepth;
        bit_2 += 8-bitdepth;
    }

    else
    {
        for ( quint16 b = 0; b < bitdepth-8; ++b )
        {
            bit_1--; bit_2--;
            lut[ bit_1 ] = 1;
            lut[ bit_2 ] = 1;
        }
    }

    float clear_mask_1  = 0, clear_mask_2   = 0;
    float xor_mask_1    = 0, xor_mask_2     = 0;

    for ( int i = 0; i < bitdepth; ++i )
    {
        if       ( lut[ bit_1+i ] == 0  ) clear_mask_1 = fOR( clear_mask_1, pow(2, i) );
        else if  ( lut[ bit_1+i ] == -1 ) xor_mask_1 = fOR( xor_mask_1, pow(2, i) );

        if       ( lut[ bit_2+i ] == 0 ) clear_mask_2 = fOR( clear_mask_2, pow(2, i) );
        else if  ( lut[ bit_2+i ] == 0 ) xor_mask_2 = fOR( xor_mask_2, pow(2, i) );
    }

    float post_bit_gain = relgain[ left_bit_slider ] * (1-mix) +
                          relgain[ right_bit_slider ]* mix;

    // RC filter params (hi/lo) -----------------------------------

    float LPF_c = pow( 0.5f, 5-m_love/25 );
    float LPF_r = pow( 0.5f, m_jive/40-0.6 );
    float HPF_c = pow( 0.5f, 5.0-m_love/32 );
    float HPF_r = pow( 0.5f, 3.0 - m_jive/40 );

    // precalc ----------------------------------------------------

    float LRC = LPF_r*LPF_c;
    float HRC = HPF_r*HPF_c;

    // @SAMPLE ====================================================

    for ( quint16 ch = 0; ch < numOutputs(); ++ch )
    {
        for( qint64 s = 0; s < nsamples; ++s )
        {
            float dry = in[ ch ][ s ];
            float s0 = 0.f;
            per_sample =  0.9995 * per_sample + 0.0005 * target_per_sample;

            // deliberately broken resampler
            // (BAD DIGITAL) -----------------------------------------
            sample_csr++;

            if ( sample_csr < next_sample && m_bad_resampler < 33150 )
                 s0 = last_sample;
            else
            {
                // for resampler - this doesn't work properly but sounds cool
                next_sample += per_sample;
                if ( m_bad_resampler == 33150. )
                     sample_csr = next_sample;

                s0 = dry*gain;
            }

            // skip gate for now -------------------------------------------
            // and shape ---------------------------------------------------
            s0 = (1+shaper_amt)*s0/(1+shaper_amt*abs(s0));

            // clamp ------------------------------------------------------
            s0 = qMax(qMin(s0, 0.95f), -0.95f);

            // bitcrush --------------------------------------------------
            if ( m_bitcrusher )
            {
                // boost to positive range: 0->255
                // SOMETHING GOES WRONG HERE
                if ( m_bitcrusher == 2 )
                     s0 = (int)((dcshift+s0) * resol ) | 0;

                // boost to positive range, -resol to +resol-1
                else s0 = (int)( s0*resol ) | 0;

                // 2s complement
                if ( s0 < 0 ) s0 = pow(2, bitdepth)+s0;
            }

            // mangle----------------------------------------------------
            if ( m_thermonuclear > 0 )
            {
                float s0A = fAND( s0, 1023-clear_mask_1 );
                s0A = fOR( fAND(s0A, 1023-xor_mask_1), fAND(1023-s0A, xor_mask_1 ));

                float s0B = fAND( s0, 1023-clear_mask_2 );
                s0B = fOR( fAND(s0B, 1023-xor_mask_2), fAND(1023-s0B, xor_mask_2 ));

                s0 = s0A * (1-mix) + s0B * mix;
            }

            if ( m_bitcrusher == 2 ) // revert
                s0 = (s0 *invresl - dcshift);
            else
            {
                // revert --------------
                s0 = (s0 * invresl );
                if ( s0 > 1.f ) s0 -= 2;
            }

            // remember last_sample
            last_sample = s0;

            // LPF ===================================
            if ( m_attitude == 1 || m_attitude == 2 )
            {
                v0 = ( 1-LRC )*v0 - LPF_c*(v1 - s0);
                v1 = ( 1-LRC )*v1 + LPF_c*v0;
                s0 = v1;
            }

            // HPF ===================================
            if ( m_attitude == 2 || m_attitude == 3 )
            {
                hv0 = ( 1-HRC )*hv0 - HPF_c*(hv1-s0);
                hv1 = ( 1-HRC )*hv1 + HPF_c*hv0;
                s0 -= hv1;
            }

            // waveshape again, just because ------------------
            s0 *= wet_gain;
            s0 = ( 1+shaper_amt_2 )*s0/( 1+shaper_amt_2*abs(s0) );

            // dcfilter --------------------------------------
            otm1 = 0.99*otm1+s0-itm1; itm1 = s0; s0 = otm1;

            // try and handle weird bit pattern supergain
            if ( m_bitcrusher ) s0 *= post_bit_gain;

            // mix ------------------------------------------
            out[ch][s] = s0+dry*dry_gain;
        }
    }

    return out;
}

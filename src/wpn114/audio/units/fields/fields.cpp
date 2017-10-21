#include "../sunits_base.hpp"
#include "../../sndfile/sndf.h"

#include <math.h>

#define ENVSIZE 16384

class fields final : public wpn114::audio::units::units_base
{
public:
    fields(uint32_t xfade_length);
    ~fields();

    void start();
    void suspend();
    void resume();
    void expose();
    void show();
    void process_midi();
    void process_audio();

private:
    float* m_buf;
    uint32_t m_samplepos;
    float m_env_samplepos;
    uin32_t m_xfade_length;
    uint32_t m_xfade_point;
    double m_env_incr;
    float m_env[ENVSIZE];

};

fields::fields(uint32_t xfade_length) :
    m_buf(0),
    m_samplepos(0),
    m_env_samplepos(0.f),
    m_xfade_length(xfade_length),
    m_env_incr(ENVSIZE/xfade_length)
{

    // load soundfile
    // load xfade envelope
    for         (int i = 0; i < ENVSIZE; ++i)
    m_env[i]    = sin(i/(float)ENVSIZE*(M_PI_2));
}

void fields::start()
{

}

void fields::suspend()
{

}

void fields::process_audio()
{
        uint32 samplepos         = unit->m_samplepos;
        float  env_samplepos     = unit->m_env_samplepos;

        const uint32 xfade_length   = unit->m_xfade_length;
        const uint32 xfade_point    = unit->m_xfade_point;
        const double xfade_incr     = unit->m_env_incr;

        //          get pointers back in position
        bufData     +=  samplepos * bufChannels;

        for(int i = 0; i < inNumSamples; ++i)
        {
            if(samplepos >= xfade_point && samplepos < bufSamples)
            {
                // if phase is in the crossfade zone
                // get data from envelope first (with linear interpolation)
                int y = floor(env_samplepos);
                float x = env_samplepos - y;

                float xfade_up = lininterp(x, unit->m_env[y], unit->m_env[y+1]);
                float xfade_down = 1 - xfade_up;

                for(int j = 0; j < bufChannels; ++j)
                {
                    out[j][i] = *bufData++ * xfade_down +
                                *(bufData - xfade_point * bufChannels) * xfade_up;
                }

                samplepos++;
                env_samplepos += xfade_incr;
            }
            else if ( samplepos == bufSamples )
            {
                // if phase reaches end of crossfade
                // main phase continues from end of 'up' xfade
                // reset the envelope phase
                bufData = unit->m_buf->data + xfade_length * bufChannels - 1;

                for         (int j = 0; j < bufChannels; ++j)
                out[j][i]   = *bufData++;

                samplepos        = xfade_length+1;
                env_samplepos    = 0;
            }
            else
            {
                // the rest: normal
                for         (int j = 0; j < bufChannels; ++j)
                out[j][i]   = *bufData++;
                samplepos++;
                // env_samplepos should be at 0;
            }
        }

        unit->m_samplepos       = samplepos;
        unit->m_env_samplepos   = env_samplepos;
    }

}

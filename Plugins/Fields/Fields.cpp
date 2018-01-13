#include "SC_PlugIn.h"
#include "SC_SndBuf.h"
#include <math.h>
#include <iostream>

#define ENVSIZE 16384
// use lininterp function from SC_SndBuf for envelope interpolation
// no need for other kinds of interpolation with envelopes, linear should be good enough

static InterfaceTable *ft;

struct Fields : public Unit
{    
    SndBuf *m_buf;
    // the buffer itself
    float m_fbufnum;
    // index of the buffer
    uint32 m_samplepos;
    // the sample position in the audio buffer (in SAMPLES: not interleaved)
    float  m_env_samplepos;
    // the sample position in the envelope buffer
    uint32 m_xfade_length;
    // the length in samples of the xfade
    uint32 m_xfade_point;
    // the point in the audio buffer where the xfade starts
    double m_env_incr;

    float m_env[ENVSIZE];
};

static void Fields_next(Fields *unit, int inNumSamples);
static void Fields_Ctor(Fields *unit);

void Fields_Ctor(Fields *unit)
{
    /* arguments are:
     * IN0(0) - buffer
     * IN0(1) - crossfade envelope size (int in ^2 samples)
       both envelopes cannot be modified after startup */    
    SETCALC(Fields_next);

    unit->m_fbufnum = -1.f;
    unit->m_buf = unit->mWorld->mSndBufs;
    unit->m_samplepos = 0;
    unit->m_env_samplepos = 0.f;

    unit->m_xfade_length = IN0(1);
    unit->m_xfade_point = unit->m_buf->samples - unit->m_xfade_length;

    // envelope phase increment,
    // a simple ratio will suffice, as it is linearly interpolated
    unit->m_env_incr = ENVSIZE/IN0(1);

    // building envelope, only one is necessary for the xfade
    // using reaper-like sine envelope
    for               (int i = 0; i < ENVSIZE; ++i)
    unit->m_env[i]  = sin(i/(float)ENVSIZE * (M_PI_2));

    Fields_next(unit, 1);
}

#define SETUP_OUT(offset) \
    if (unit->mNumOutputs != bufChannels) { \
        ClearUnitOutputs(unit, inNumSamples); \
        return; \
    } \
    float *const * const out = &OUT(offset);

void Fields_next(Fields *unit, int inNumSamples)
{
    GET_BUF;
    SETUP_OUT(0);

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

PluginLoad(Fields)
{
    ft = inTable;
    DefineSimpleUnit(Fields);
}





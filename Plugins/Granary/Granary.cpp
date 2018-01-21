#include "SC_PlugIn.h"
#include "SC_SndBuf.h"
#include <math.h>
#include <iostream>

#define ENVSIZE 16384
#define NGRAINS_MAX 16
#define NVOICES_MAX 4
// use lininterp function from SC_SndBuf for envelope interpolation
// no need for other kinds of interpolation with envelopes, linear should be good enough

static InterfaceTable *ft;

struct Granary : public Unit
{    
    SndBuf  *m_buf;
    float   m_fbufnum;    

    float   m_env[ENVSIZE];    
    float   m_phase;
    float   m_envphase;
};

static void Granary_next(Granary *unit, int inNumSamples);
static void Granary_Ctor(Granary *unit);

void Granary_Ctor(Granary *unit)
{
    /* arguments are:
     * IN0(0) - buffer
     * IN0(1) - x / startpos
     * IN0(2) - y / channel position
     * IN0(3) - rate
     * IN0(4) - overlap
     * IN0(5) - pan
     * IN0(6) - rate noise
     */

    SETCALC(Granary_next);

    unit->m_fbufnum     = -1.f;
    unit->m_buf         = unit->mWorld->mSndBufs;

    unit->m_phase       = 0.f;
    unit->m_envphase    = 0.f;

    // building envelope, only one is necessary for the xfade
    // using reaper-like sine envelope
    for               (int i = 0; i < ENVSIZE; ++i)
    unit->m_env[i]  = sin(i/(float)ENVSIZE * (M_PI_2));

    Granary_next(unit, 1);
}

#define SETUP_OUT(offset) \
    if (unit->mNumOutputs != bufChannels) { \
        ClearUnitOutputs(unit, inNumSamples); \
        return; \
    } \
    float *const * const out = &OUT(offset);

void Granary_next(Granary *unit, int inNumSamples)
{
    GET_BUF;
    SETUP_OUT(0);

    float phase         = unit->m_phase;
    float envphase      = unit->m_envphase;

    float x             = IN0(1);
    float y             = IN0(2);
    float rate          = IN0(3);
    float overlap       = IN0(4);
    float pan           = IN0(5);
    float noiserate     = IN0(6);

    //          get pointers back in position
    bufData     +=  samplepos * bufChannels;

    for(int i = 0; i < inNumSamples; ++i)
    {


    }

    unit->m_phase       = phase;
    unit->m_envphase    = envphase;
}

PluginLoad(Granary)
{
    ft = inTable;
    DefineSimpleUnit(Granary);
}





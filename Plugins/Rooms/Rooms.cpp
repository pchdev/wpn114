#include "SC_PlugIn.h"
#include "SC_SndBuf.h"
#include <math.h>
#include <iostream>

static InterfaceTable *ft;

// we use interleaved buffers for speaker configuration, as follows:
// [0] -> x
// [1] -> y
// [2] -> r
// [3] -> l
#define LSX bufData[0]
#define LSY bufData[1]
#define LSR bufData[2]
#define LSL bufData[3]

struct Rooms : public Unit
{    
    float       m_fbufnum;
    SndBuf*     m_buf;
    uint8_t     m_nout;
};

struct Rooms2: public Rooms {};

static void Rooms_next(Rooms *unit, int inNumSamples);
static void Rooms2_next(Rooms2* unit, int inNumSamples);
static void Rooms_Ctor(Rooms *unit);
static void Rooms2_Ctor(Rooms2 *unit);

void Rooms_Ctor(Rooms *unit)
{
    SETCALC(Rooms_next);
    // arguments are:
    // IN0(0) - bufnum
    // IN0(1) - num_outputs
    // IN(2)  - input
    // IN0(3) - src_x
    // IN0(4) - src_y
    unit->m_buf = unit->mWorld->mSndBufs;
    unit->m_fbufnum = -1.f;
    unit->m_nout = IN0(1);

    Rooms_next(unit, 1);
}

void Rooms2_Ctor(Rooms2 *unit)
{
    SETCALC(Rooms2_next);
    // arguments are:
    // IN0(0) - bufnum
    // IN0(1) - num_outputs
    // IN(2)  - input_1
    // IN(3)  - input_2
    // IN0(4) - src1_x
    // IN0(5) - src1_y
    // IN0(6) - src2_x
    // IN0(7) - src2_y

    unit->m_buf = unit->mWorld->mSndBufs;
    unit->m_fbufnum = -1.f;
    unit->m_nout = IN0(1);

    Rooms2_next(unit, 1);
}

inline float gain(
        // main calculation function
        const float src_x, const float src_y,
        const float ls_x, const float ls_y,
        const float ls_r, const float ls_l)
{
    float dx = fabs(ls_x - src_x);
    if (dx > ls_r) return 0.f;
    float dy = fabs(ls_y - src_y);
    if (dy > ls_r) return 0.f;

    return fabs((1 - (sqrt((dx*dx) + (dy*dy)) / ls_r))  * ls_l);
}

#define SETUP_OUT(offset) \
    float *const *const out = &OUT(offset)

void Rooms_next(Rooms *unit, int inNumSamples)
{
    GET_BUF;
    SETUP_OUT(0);

    float* in        = IN(2);
    float  src_x     = IN0(3);
    float  src_y     = IN0(4);
    uint8_t nout     = unit->m_nout;

    float coeffs[nout];
    for (int n = 0; n < nout; ++n)
    {
        coeffs[n]   = gain(src_x, src_y, LSX, LSY, LSR, LSL);
        bufData     += 4;
    }

    for(int i = 0; i < inNumSamples; ++i)
        for(int n = 0; n < nout; ++n)
            out[n][i] = in[i] * coeffs[n];
}

void Rooms2_next(Rooms2 *unit, int inNumSamples)
{
    GET_BUF;
    SETUP_OUT(0);

    float* in_l      = IN(2);
    float* in_r      = IN(3);

    float  src_1[2]  = { IN0(4), IN0(5) };
    float  src_2[2]  = { IN0(6), IN0(7) };

    uint8_t nout     = unit->m_nout;

    float coeffs[2][nout];

    for (int n = 0; n < nout; ++n)
    {
        coeffs[0][n]   = gain(src_1[0], src_1[1], LSX, LSY, LSR, LSL);
        coeffs[1][n]   = gain(src_2[0], src_2[1], LSX, LSY, LSR, LSL);

        bufData     += 4;
    }

    for(int i = 0; i < inNumSamples; ++i)
        for(int n = 0; n < nout; ++n)
            out[n][i] = in_l[i] * coeffs[0][n] + in_r[i] * coeffs[1][n];
}

PluginLoad(Rooms)
{
    ft = inTable;    
    DefineSimpleCantAliasUnit(Rooms);
}

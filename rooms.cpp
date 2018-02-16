#include "rooms.h"
#include <math.h>

RoomsSetup::RoomsSetup()
{

}

RoomsSetup::~RoomsSetup()
{

}

uint16_t RoomsSetup::numInputs() const
{
    return m_ninputs;
}

uint16_t RoomsSetup::numOutputs() const
{
    return m_noutputs;
}

void RoomsSetup::setNumInputs(const uint16_t ninputs)
{
    m_ninputs = ninputs;
}

void RoomsSetup::setNumOutputs(const uint16_t noutputs)
{
    m_noutputs = noutputs;
}


Rooms::Rooms() : m_setup(0)
{
    SETN_IN     (0);
    SETN_OUT    (0);
}

Rooms::~Rooms() {}
void Rooms::classBegin() {}
void Rooms::componentComplete()
{
    INITIALIZE_AUDIO_IO;
    if ( ! m_setup ) return;
}

inline float spgain(const float src_x, const float src_y,
                    const float ls_x, const float ls_y,
                    const float ls_r, const float ls_l)
{
    float dx = fabs(ls_x - src_x);
    if ( dx > ls_r ) return 0.f;
    float dy = fabs(ls_y - src_y);
    if ( dy > ls_r) return 0.f;

    return fabs((1 - (sqrt((dx*dx) + (dy*dy)) / ls_r)) * ls_l);
}

RoomsSetup* Rooms::setup() const
{
    return m_setup;
}

void Rooms::setSetup(RoomsSetup *setup)
{
    m_setup = setup;
}

float** Rooms::process(const quint16 nsamples)
{
    uint16_t nout   = m_num_outputs;
    auto out        = OUT;

    ZEROBUF(IN, m_num_inputs); GET_INPUTS;

    float coeffs    [nout];

    for ( int i = 0; i < nin; ++i )
        for ( int o = 0; o < nout; ++o )
            //coeffs[o] = spgain(srcx, srcy, LSX, LSY, LSR, LSL);

    for ( int s = 0; s < nsamples; ++s )
        for ( int n = 0; n < nin; ++ n )
            for ( int o = 0; o < nout; ++ o )
                out[o][s] = in[n][s];

}



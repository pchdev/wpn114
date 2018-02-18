#include "rooms.h"
#include <math.h>

// ROOMS_ELEMENT -------------------------------------------------------------------

RoomsElement::RoomsElement() {}
RoomsElement::~RoomsElement() {}

float RoomsElement::level() const
{
    return m_level;
}

float RoomsElement::influence() const
{
    return m_influence;
}

QVector2D RoomsElement::position() const
{
    return m_position;
}

void RoomsElement::setLevel(const float level)
{
    m_level = level;
}

void RoomsElement::setInfluence(const float influence)
{
    m_influence = influence;
}

void RoomsElement::setPosition(const QVector2D position)
{
    m_position = position;
}

// ROOMS_SOURCE ------------------------------------------------------------------

QQmlListProperty<AudioObject> Source::inputs()
{
    return QQmlListProperty<AudioObject>(this, m_inputs);
}

QList<AudioObject*> Source::get_inputs()
{
    return m_inputs;
}

QVector2D Source::lposition() const
{
    return m_lposition;
}

QVector2D Source::rposition() const
{
    return m_rposition;
}

void Source::setLposition(const QVector2D lposition)
{
    m_lposition = lposition;
}

void Source::setRPosition(const QVector2D rposition)
{
    m_rposition = rposition;
}

// ROOMS_SETUP -------------------------------------------------------------------

RoomsSetup::RoomsSetup() {}
RoomsSetup::~RoomsSetup() {}

uint16_t RoomsSetup::numOutputs() const
{
    return m_noutputs;
}

void RoomsSetup::setNumOutputs(const uint16_t noutputs)
{
    m_noutputs = noutputs;
}

QQmlListProperty<RoomsElement> RoomsSetup::speakers()
{
    return QQmlListProperty<RoomsElement>(this, m_speakers);
}

QList<RoomsElement*> RoomsSetup::get_speakers()
{
    return m_speakers;
}

// ROOMS_AUDIO -------------------------------------------------------------------


Rooms::Rooms() : m_setup(0) {}

Rooms::~Rooms() {}
void Rooms::classBegin() {}

void Rooms::componentComplete()
{    
    if ( ! m_setup ) return;

    uint16_t    nin = 0;
    SETN_OUT    ( m_setup->numOutputs() );

    for( const auto& src : m_sources )
    {
        uint16_t maxchannels = 0;

        for ( const auto& subsrc : src->get_inputs() )
        {
            maxchannels = qMax<uint16_t>( maxchannels, subsrc->numOutputs() );

            // add independent channel positions to pointer vector
            if ( subsrc->numOutputs() == 1 )
            {
                float* data_ptrs[4]  = { &src->position().x(), &src->position().y()
                                         &src->influence(), &src->level() };

                m_src_data.push_back(data_ptrs);
            }

            else if ( subsrc->numOutputs() == 2 )
            {
                float* data_ptrs_l[4]  = { &src->lposition().x(), &src->lposition().y()
                                         &src->influence(), &src->level() };
                float* data_ptrs_r[4]  = { &src->rposition().x(), &src->rposition().y()
                                           &src->influence(), &src->level() };

                m_src_data.push_back(data_ptrs_l);
                m_src_data.push_back(data_ptrs_r);
            }
        }

        nin += maxchannels;
    }

    SETN_IN ( nin );
    INITIALIZE_AUDIO_IO;
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

QQmlListProperty<Source> Rooms::sources()
{
    return QQmlListProperty<Source>(this, m_sources);
}

float**& Rooms::get_inputs(const quint64 nsamples)
{
    // sends are not allowed here
    for(const auto& source : m_sources )
    {
        for(const auto& input: source->get_inputs())
        {
            if ( input->active() )
            {
                uint16_t unout = input->numOutputs();
                uint16_t uoff  = input->offset();
                float** buf = input->process(nsamples);
                inbufmerge(IN, buf, m_num_inputs, unout, uoff, nsamples, source->level());
            }
        }
    }
}

#define SRCX spos[i][0]
#define SRCY spos[i][1]
#define SRCR spos[i][2]
#define SRCL spos[i][3]

#define LSX speakers[o]->position().x()
#define LSY speakers[o]->position().y()
#define LSR speakers[o]->influence()
#define LSL speakers[o]->level()

float** Rooms::process(const quint16 nsamples)
{
    uint16_t nout   = m_num_outputs;
    uint16_t nin    = m_num_inputs;
    auto out        = OUT;
    auto speakers   = m_setup->get_speakers();
    auto spos       = m_src_data;

    ZEROBUF         ( IN, m_num_inputs );

    float**& in =   get_inputs(nsamples);
    float coeffs    [ nin ] [ nout ];

    for (int i = 0; i < nin; ++i)
        for(int o = 0; i < nout; ++o)
            coeffs[i][o] = spgain ( SRCX, SRCY, LSX, LSY, LSR, LSL );

    for ( int s = 0; s < nsamples; ++s )
        for ( int n = 0; n < nin; ++n )
            for ( int ch = 0; ch < nout; ++ch)
                out[ch][s] += in[n][s] * coeffs[ch];

}

#include "rooms.hpp"
#include <math.h>

// ROOMS_ELEMENT -------------------------------------------------------------------

RoomsObject::RoomsObject() {}
RoomsObject::~RoomsObject() {}

float RoomsObject::level() const
{
    return m_level;
}

float RoomsObject::influence() const
{
    return m_influence;
}

QVector2D RoomsObject::position() const
{
    return m_position;
}

void RoomsObject::setLevel(const float level)
{
    m_level = level;
}

void RoomsObject::setInfluence(const float influence)
{
    m_influence = influence;
}

void RoomsObject::setPositions(const QVector<qreal> positions)
{
    m_positions = positions;
}

// ROOMS_SPEAKER -----------------------------------------------------------------

Speaker::Speaker() {}
SpeakerPair::SpeakerPair() {}
SpeakerRing::SpeakerRing() {}


int SpeakerObject::output() const
{
    return m_output;
}

void SpeakerObject::setOutput(const int output)
{
    m_output = output;
}

void SpeakerPair::classBegin() {}
void SpeakerRing::classBegin() {}

void SpeakerPair::componentComplete()
{

}

Speaker::Axis SpeakerPair::axis() const
{
    return m_axis;
}

qreal SpeakerPair::offset() const
{
    return m_offset;
}

QVector<qreal> SpeakerPair::offsets() const
{
    return m_offsets;
}

void SpeakerRing::componentComplete()
{
    for ( int i = 0; i < m_nspeakers; ++i )
    {
        m_positions.push_back((sin((qreal)i/m_nspeakers*M_PI*2) + 1.f) / 2.f);
        m_positions.push_back((cos((qreal)i/m_nspeakers*M_PI*2) + 1.f) / 2.f);
    }
}

qreal SpeakerRing::offset() const
{
    return m_offset;
}

Speaker::Order SpeakerRing::order() const
{
    return m_order;
}

quint16 SpeakerRing::nspeakers() const
{
    return m_nspeakers;
}

// ROOMS_SOURCE ------------------------------------------------------------------

void Source::classBegin() {}
void Source::componentComplete()
{
    quint16 maxchannels = 0;
    for ( const auto& src : m_inputs )
        maxchannels = qMax<quint16>(maxchannels, src->numOutputs());

    m_nchannels = maxchannels;
}


void Source::setLevel(const qreal level)
{
    m_level = level;
}

void Source::setInfluence(const qreal influence )
{
    m_influence = influence;
}

void Source::setPositions(const QVector<qreal> positions)
{
    m_positions = positions;
}

qreal Source::level() const
{
    return m_level;
}

qreal Source::influence() const
{
    return m_influence;
}

QVector<qreal> Source::positions() const
{
    return m_positions;
}

QQmlListProperty<AudioObject> Source::inputs()
{
    return QQmlListProperty<AudioObject>(this, m_inputs);
}

QList<AudioObject*> Source::get_inputs()
{
    return m_inputs;
}

quint16 Source::nchannels() const
{
    return m_nchannels;
}

// ROOMS_SETUP -------------------------------------------------------------------

RoomsSetup::RoomsSetup() : m_noutputs(0) {}
RoomsSetup::~RoomsSetup() {}

void RoomsSetup::classBegin() {}
void RoomsSetup::componentComplete()
{
    for ( const auto& speakerobj : m_speakers )
        m_noutputs += speakerobj->nspeakers();
}

uint16_t RoomsSetup::numOutputs() const
{
    return m_noutputs;
}

QQmlListProperty<SpeakerObject> RoomsSetup::speakers()
{
    return QQmlListProperty<SpeakerObject>(this, m_speakers);
}

QList<SpeakerObject *> &RoomsSetup::get_speakers()
{
    return m_speakers;
}

void RoomsSetup::appendSpeaker(QQmlListProperty<SpeakerObject> *list, SpeakerObject *speaker)
{
    reinterpret_cast<RoomsSetup*>(list->data)->appendSpeaker(speaker);
}

void RoomsSetup::appendSpeaker(SpeakerObject *speaker)
{
    m_speakers.append(speaker);

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
        nin += src->nchannels();

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

    return fabs( (1 - (sqrt((dx*dx) + (dy*dy)) / ls_r)) * ls_l );
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

inline float**& Rooms::get_inputs(const quint64 nsamples)
{
    // sends are not allowed here
    for ( const auto& source : m_sources )
        for ( const auto& input: source->get_inputs() )
            if ( input->active() )
            {
                uint16_t unout = input->numOutputs();
                uint16_t uoff  = input->offset();
                float** buf = input->process(nsamples);
                inbufmerge(IN, buf, m_num_inputs, unout, uoff, nsamples, source->level());
            }

    return IN;
}

#define SRCX sources[src]->positions()[ch*1]
#define SRCY sources[src]->positions()[ch*2]
#define SRCR sources[src]->influence()
#define SRCL sources[src]->level()

#define LSX speakers[o]->position().x()
#define LSY speakers[o]->position().y()
#define LSR speakers[o]->influence()
#define LSL speakers[o]->level()

float** Rooms::process(const quint16 nsamples)
{
    uint16_t nout               = m_num_outputs;
    uint16_t nin                = m_num_inputs;
    auto out                    = OUT;
    const auto& speakers        = m_setup->get_speakers();
    auto sources                = m_sources;

    ZEROBUF         ( IN, m_num_inputs );

    float**& in =   get_inputs( nsamples );
    float coeffs    [ nin ] [ nout ];

    uint16_t channel = 0;

    for ( int src = 0; src < sources.size(); ++src )
        for ( int ch = 0; ch < sources[src]->nchannels(); ++ch )
        {
            for ( int o = 0; o < nout; ++ o )
                coeffs[channel][o] = spgain ( SRCX, SRCY, LSX, LSY, LSR, LSL );
            channel++;
        }

    for ( int s = 0; s < nsamples; ++s )
        for ( int n = 0; n < nin; ++n )
            for ( int ch = 0; ch < nout; ++ch)
                out[ch][s] += in[n][s] * coeffs[n][ch];

}

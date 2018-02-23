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

QVector<qreal> RoomsObject::positions() const
{
    return m_positions;
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

void SpeakerPair::setAxis(const Axis axis)
{
    m_axis = axis;
}

void SpeakerPair::setOffset(const qreal offset)
{
    m_offset = offset;
}

void SpeakerPair::setOffsets(const QVector<qreal> offsets )
{
    m_offsets = offsets;
}

void SpeakerRing::componentComplete()
{
    for ( int i = 0; i < m_nspeakers; ++i )
    {
        m_positions.push_back((sin((float)i/m_nspeakers*M_PI*2) + 1.f) / 2.f);
        m_positions.push_back((cos((float)i/m_nspeakers*M_PI*2) + 1.f) / 2.f);
    }

    qDebug() << m_positions;
}

qreal SpeakerRing::offset() const
{
    return m_offset;
}

Speaker::Order SpeakerRing::order() const
{
    return m_order;
}

void SpeakerRing::setOffset(const qreal offset)
{
    m_offset = offset;
}

void SpeakerRing::setOrder(const Order order)
{
    m_order = order;
}

void SpeakerRing::setNspeakers(const quint16 nspeakers)
{
    m_nspeakers = nspeakers;
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
    {
        quint16 nspeakers = speakerobj->nspeakers();
        m_noutputs += nspeakers;

        for (int i = 0; i < nspeakers; ++i )
        {
            SpeakerChannel sc;
            sc.x = speakerobj->positions()[ i*2 ];
            sc.y = speakerobj->positions()[ i*2+1 ];
            sc.r = speakerobj->influence();
            sc.l = speakerobj->level();
            m_channels << sc;
        }
    }
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

QVector<RoomsSetup::SpeakerChannel> RoomsSetup::get_channels() const
{
    return m_channels;
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


Rooms::Rooms() : m_setup(0)
{
    SETN_IN     ( 0 );
    SETN_OUT    ( 0 );
    SET_OFFSET  ( 0 );
}

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

// TODO! implement source influence...
inline float spgain(const float srx, const float sry,
                    const float lsx, const float lsy,
                    const float lsr, const float lsl)
{
    float dx = fabs(srx - lsx);
    if ( dx > lsr ) return 0.f;
    float dy = fabs(sry - lsy);
    if ( dy > lsr ) return 0.f;

    float d = sqrt((dx*dx)+(dy*dy));

    if ( d/lsr > 1.f ) return 0.f;
    else return ( 1.f - d/lsr ) * lsl;
}

#define SRCX sources[src]->positions()[ ch*2 ]
#define SRCY sources[src]->positions()[ ch*2+1 ]
#define SRCR sources[src]->influence()
#define SRCL sources[src]->level()

#define LSX speakers[o].x
#define LSY speakers[o].y
#define LSR speakers[o].r
#define LSL speakers[o].l

float** Rooms::process(const quint16 nsamples)
{
    uint16_t nout               = m_num_outputs;
    uint16_t nin                = m_num_inputs;
    auto out                    = OUT;
    const auto& speakers        = m_setup->get_channels();
    auto sources                = m_sources;

    ZEROBUF   ( IN, nin );
    ZEROBUF   ( OUT, nout );

    auto in         = get_inputs( nsamples );
    float coeffs    [ nin ] [ nout ];

    uint16_t channel = 0;

    for ( int src = 0; src < sources.size(); ++src )
        for ( int ch = 0; ch < sources[src]->nchannels(); ++ch )
        {
            for ( int o = 0; o < nout; ++ o )
                coeffs[channel][o] = spgain ( SRCX, SRCY, LSX, LSY, LSR, LSL );
            channel++;
        }

    for ( int ch = 0; ch < nout; ++ch )
        for ( int n = 0; n < nin; ++n )
            for ( int s = 0; s <nsamples; ++ s )
                out[ch][s] += in[n][s] * coeffs[n][ch];

    return out;
}

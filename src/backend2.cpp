#include "backend2.hpp"

World World::m_instance = World();

//---------------------------------------------------------------------------------------------------

float WAOElement::level() const
{
    return m_level;
}

uint16_t WAOElement::offset() const
{
    return m_offset;
}

void WAOElement::set_level(float level)
{
    m_level = level;
}

void WAOElement::set_offset(uint16_t offset)
{
    m_offset = offset;
}

//---------------------------------------------------------------------------------------------------
// OUT_ELEMENT
//---------------------------------------------------------------------------------------------------

QQmlListProperty<WASndElement> WAOElement::sends()
{
    return QQmlListProperty<WASndElement>(this, m_sends);
}

QList<WASndElement*> WAOElement::get_sends() const
{
    return m_sends;
}

uint16_t WAOElement::n_outputs() const
{
    return m_n_outputs;
}

void WAOElement::setn_outputs(uint16_t n_outputs)
{
    m_n_outputs = n_outputs;
}

//---------------------------------------------------------------------------------------------------
// IN_ELEMENT
//---------------------------------------------------------------------------------------------------

QQmlListProperty<WAOElement> WAIElement::inputs()
{
    return QQmlListProperty<WAOElement>(this, m_inputs);
}

uint16_t WAIElement::n_inputs() const
{
    return m_n_inputs;
}

void WAIElement::setn_inputs(uint16_t n_inputs)
{
    m_n_inputs = n_inputs;
}

void WAIElement::add_receive(const WASndElement &send)
{
    m_receives.push_back(&send);
}

//---------------------------------------------------------------------------------------------------
// IN_OUT_ELEMENT
//---------------------------------------------------------------------------------------------------

bool WAIOElement::bypassed() const
{
    return m_bypassed;
}

void WAIOElement::set_bypassed(bool bypassed)
{
    m_bypassed = bypassed;
}

//---------------------------------------------------------------------------------------------------
// SEND_ELEMENT
//---------------------------------------------------------------------------------------------------

WASndElement::WASndElement() : m_target(0)
{
    m_emitter = qobject_cast<WAOElement*>(parent);
}

WASndElement::~WASndElement() {}

ON_COMPONENT_COMPLETED ( WASndElement )
{
    m_target->add_receive(*this);
}

void WASndElement::process(float **&, const uint16_t) {}
// nothing to process

bool WASndElement::prefader() const
{
    return m_prefader;
}

bool WASndElement::postfx() const
{
    return m_postfx;
}

WAIElement* const& WASndElement::to() const
{
    return m_target;
}

WAOElement* const& WASndElement::from() const
{
    return m_emitter;
}

void WASndElement::set_target(WAIElement* target)
{
    m_target = target;
}

void WASndElement::set_postfx(bool postfx)
{
    m_postfx = postfx;
}

void WASndElement::set_prefader(bool prefader)
{
    m_prefader = prefader;
}

//---------------------------------------------------------------------------------------------------
// STREAMS
//---------------------------------------------------------------------------------------------------

WAStream::WAStream(const WAElement& outfall) :
    has_downstreams(false),
    has_upstreams(false)
{
    outfall.set_stream      ( *this );
    m_aelements.push_back   ( &outfall );

    if  ( auto out = qobject_cast<WAOElement*>( &outfall ))
    {
        m_n_outputs = out->n_outputs();
        m_offset = out->offset();
    }

    else m_n_outputs = 0;
}

void WAStream::push_front(const WAElement &node)
{
    node.set_stream         ( *this );
    m_aelements.push_front  ( &node );
}

const WAElement& WAStream::upstream()
{
    if ( !m_aelements.empty() ) return *m_aelements.first();
}

const WAElement& WAStream::downstream()
{
    if ( !m_aelements.empty() ) return *m_aelements.last();
}

void WAStream::alloc_pool(const uint16_t nsamples)
{
    //! TODO
}

void WAStream::resolve()
{

}

inline bool WAStream::begins_with( const WAElement& element)
{
    WAIElement* in  = qobject_cast<WAIElement*>( &element );

    //  if element has multiple inputs
    if ( in && ( in->children().size() > 1 ||
         !in->has_receives())) return true;

    else return false;
}

WAStream::~WAStream()
{

}

WAStream::process(float **&, const uint16_t nsamples)
{

}

//---------------------------------------------------------------------------------------------------
// STREAM_FACTORY
//---------------------------------------------------------------------------------------------------

#define wao_cast ( waelement ) qobject_cast<WAOElement*>( &waelement )
#define wai_cast ( waelement ) qobject_cast<WAIElement*>( &waelement )
#define wae_cast ( qobject )   qobject_cast<WAElement*>( qobject )

WAStreamMaker::WAStreamMaker() {}

inline bool has_direct_upstream( const WAElement& element)
{
    return !element.children().empty();
}

inline bool has_indirect_upstream( const WAElement& element)
{
    auto wai = wai_cast ( element );
    return wai && wai->has_receives();
}

void WAStreamMaker::upstream(const WAStream &stream)
{
    WAElement& target = stream.upstream();

    if  ( !has_direct_upstream ( target) )
    {
        // stream is complete, no further parsing required
        stream.has_upstreams = has_indirect_upstream ( target );
        m_streams.push_back ( stream );
        return;
    }
    else if ( stream.has_upstreams = WAStream::begins_with ( target ))
    {
        m_streams.push_back ( stream );

        // stream is complete, but there are others upstream
        // continue parsing
        for ( const auto& child : target.children() )
            upstream ( *wae_cast ( child ) );
    }
    else
    {
        for ( const auto& child : target.children() )
        {
            // stream is not complete yet

            // we add child to the stream as the new first node
            // and continue to parse upstream...
            auto element = *wae_cast ( child );
            stream.push_front ( element );
            upstream ( stream );
        }
    }
}

void WAStreamMaker::upstream(const WAElement &outfall)
{
    auto stream = new WAStream( outfall );
    stream->has_downstreams = wao_cast ( outfall );

    upstream( stream );

    // note a stream's outfall is not necessarily worldstream
    // it can be an analyzer (vu, freqscope...)
}

#define resolve_inputs( target_container, getter_func )         \
    int i = 0;                                                  \
    for ( const auto& input : target_container )                \
    {                                                           \
        auto stream_target = input->##getter_func;              \
        int idx = m_streams.indexOf ( stream_target );          \
        m_streams.move ( idx, i );                              \
        i++;                                                    \
        resolve ( &stream_target );                             \
    }                                                           \

void WAStreamMaker::resolve(WAStream const& stream)
{
    if(stream.has_upstreams)
    {
        WAIElement* up = wai_cast(stream.upstream());

        resolve_inputs ( up->get_inputs(), stream() );
        resolve_inputs ( up->receives(), from()->stream() );
    }
}

void WAStreamMaker::resolve_streams()
{
    // at this point, we should have all the separated streams in vector
    for ( const auto& stream : m_streams )
        resolve(*stream);
}

QVector<WAStream*> WAStreamMaker::streams() const
{
    return m_streams;
}

//---------------------------------------------------------------------------------------------------
// WORLD
//---------------------------------------------------------------------------------------------------

World& World::instance()
{
    return m_singleton;
}

World::World() : m_n_inputs(0), m_n_outputs(0), m_output(0)
{
    open(QIODevice::ReadOnly);
}

World::~World() {}

ON_COMPONENT_COMPLETED ( World )
{
    configure();

    WAStreamMaker factory;

    for ( const auto& aelement : m_aelements )
        factory.upstream ( *aelement );

    // + TODO: WORLDSTREAM
}

void World::configure()
{
    m_format.setCodec           ( "audio/pcm" );
    m_format.setByteOrder       ( QAudioFormat::LittleEndian );
    m_format.setSampleType      ( QAudioFormat::SignedInt );
    m_format.setSampleSize      ( 16 );
    m_format.setSampleRate      ( World::instance().samplerate() );
    m_format.setChannelCount    ( m_n_outputs );

    auto device_info = QAudioDeviceInfo::defaultOutputDevice();

    if ( m_device != "" || m_device != "default" )
    {
        auto devices = device_info.availableDevices( QAudio::AudioOutput );
        for ( const auto& device : devices )
        {
            if (device.deviceName() == m_device)
            {
                qDebug() << device.deviceName();
                device_info = device;
                break;
            }
        }
    }

    if( !device_info.isFormatSupported(m_format) )
        qDebug() << "audio format not supported";

    m_output = new QAudioOutput(device_info, m_format);

    connect ( m_output, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(onAudioStateChanged(QAudio::State)) );

}

void World::onAudioStateChanged(QAudio::State state)
{
    qDebug() << state;
}

QQmlListProperty<WAElement> World::aelements()
{
    return QQmlListProperty<WAElement>(this, m_aelements);
}

void World::process(float **&, const uint16_t nsamples)
{

}

void World::run()
{
    m_output  -> start( this );
    qDebug()  << m_output->error();
}

void World::stop()
{
    m_output->stop();
}

qint64 World::readData(char* data, qint64 maxlen)
{
    quint16 nout        = m_n_outputs;
    float level         = m_level;
    quint16 bsize       = m_blocksize;
    float* bufdata      = m_intl_buffer;

    float**& wend       = w.run(bsize);

    // zero out buffer
    std::memset(bufdata, 0.f, sizeof(float)*bsize*nout);

    for ( quint16 ch = 0; ch < nout; ++ch )
        for ( quint16 s = 0; s < bsize; ++ s )
            *bufdata++ = wend[ch][s] * level;

    // reset pointer when done
    bufdata -= bsize*nout;

    //  little endian 16bits signed integer byte cast for QAudioOutput
    for (quint16 i = 0; i < bsize*nout; ++i)
    {
        qint16 sdata = static_cast<qint16>( bufdata[i] * 32767 );
        qToLittleEndian<qint16>(sdata, data);
        data += 2;
    }

    // i.e. block size * 2bytes per value * numChannels
    return ( bsize*2*nout );
}

qint64 World::writeData(const char* data, qint64 maxlen)
{
    Q_UNUSED ( data )
    Q_UNUSED ( maxlen )

    return 0;
}

qint64 World::bytesAvailable()
{
    return 0;
}

uint32_t World::samplerate() const
{
    return m_samplerate;
}

uint16_t World::blocksize() const
{
    return m_blocksize;
}

QString World::device() const
{
    return m_device;
}

void World::set_samplerate(uint32_t samplerate)
{
    m_samplerate = samplerate;
}

void World::set_blocksize(uint16_t blocksize)
{
    m_blocksize = blocksize;
}

void World::set_device(QString device)
{
    m_device = device;
}

void World::get_aelements() const
{
    return m_aelements;
}

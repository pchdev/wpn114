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

WASndElement::WASndElement() {}
WASndElement::~WASndElement() {}

ON_COMPONENT_COMPLETED ( WASndElement )
{
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

WAIElement* WASndElement::target()
{
    return m_target;
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

WAStream::WAStream(WAElement* const& outfall)
{
    m_aelements.push_back(outfall);
    m_n_outputs = outfall->n_outputs();
}

const WAElement& WAStream::first()
{
    if ( !m_aelements.empty() ) return *m_aelements[0];
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

WAStreamFactory::WAStreamFactory() {}

QVector<WAStream*> WAStreamFactory::upstream(const WAStream &stream)
{

}

inline bool can_upstream(const WAElement& outfall)
{
    bool can = true;

    WAOElement* sends = qobject_cast<WAOElement*>( outfall );
    WAOElement* rcvs = qobject_cast<WAIElement*>( outfall );

    if ( !sends && !rcvs ) return can;
    if ( sends && !sends->get_sends().empty() ) return false;
    if ( rcvs && !rcvs->get_receives().empty() ) return false;

    return can;
}

QVector<WAStream*> WAStreamFactory::upstream(const WAElement &outfall)
{
    WAStream* stream    = new WAStream( &outfall );

    QVector<WAStream*>  stream_vec { stream };

    // note a stream's outfall is not necessarily worldstream
    // it can be an analyzer (vu, freqscope...)

    // we now have to go upstream again
    // to check for other WAElements up the line

    // /!\ important: if WAElement receives from a WASndElement, the stream is complete
    // same if the WAElement sends to another
    // this marks the end of the stream

    if ( can_upstream( outfall ) ) stream_vec += upstream(*stream);
    else return stream_vec;

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

    WAStreamFactory factory;

    for ( const auto& aelement : m_aelements )
        m_streams += factory.upstream ( *aelement );
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
    m_output->start( this );
    qDebug() << m_output->error();
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

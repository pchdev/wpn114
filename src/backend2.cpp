#include "backend2.hpp"

using namespace wpn114::audio;
World World::m_instance = World();

//---------------------------------------------------------------------------------------------------

float AAbstractElement::level() const
{
    return m_level;
}

uint16_t AAbstractElement::offset() const
{
    return m_offset;
}

bool AAbstractElement::muted() const
{
    return m_muted;
}

bool AAbstractElement::active() const
{
    return m_active;
}

void AAbstractElement::set_level(float level)
{
    m_level = level;
}

void AAbstractElement::set_active(bool active)
{
    m_active = active;
}

void AAbstractElement::set_muted(bool muted)
{
    m_muted = muted;
}

void AAbstractElement::set_offset(uint16_t offset)
{
    m_offset = offset;
}

//---------------------------------------------------------------------------------------------------
// AElement
//---------------------------------------------------------------------------------------------------

AElement::~AElement()
{

}

QQmlListProperty<ASendElement> AElement::sends()
{
    return QQmlListProperty<ASendElement>(this, m_sends);
}

uint16_t AElement::n_outputs() const
{
    return m_n_outputs;
}

void AElement::setn_outputs(uint16_t n_outputs)
{
    m_n_outputs = n_outputs;
}

//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------

AProcessorElement::~AProcessorElement()
{

}

QQmlListProperty<AElement> AProcessorElement::inputs()
{
    return QQmlListProperty<AElement>(this, m_inputs);
}

uint16_t AProcessorElement::n_inputs() const
{
    return m_n_inputs;
}

void AProcessorElement::setn_inputs(uint16_t n_inputs)
{
    m_n_inputs = n_inputs;
}

//---------------------------------------------------------------------------------------------------

ASendElement::ASendElement() {}
ASendElement::~ASendElement() {}

ON_COMPONENT_COMPLETED ( ASendElement )
{

}

bool ASendElement::prefader() const
{
    return m_prefader;
}

bool ASendElement::postfx() const
{
    return m_postfx;
}

AProcessorElement* ASendElement::target()
{
    return m_target;
}

void ASendElement::set_target(AProcessorElement *target)
{
    m_target = target;
}

void ASendElement::set_postfx(bool postfx)
{
    m_postfx = postfx;
}

void ASendElement::set_prefader(bool prefader)
{
    m_prefader = prefader;
}

//---------------------------------------------------------------------------------------------------
// WORLD
//---------------------------------------------------------------------------------------------------

World& World::instance()
{
    return m_instance;
}

uint32_t World::samplerate() const
{
    return m_samplerate;
}

uint16_t World::blocksize() const
{
    return m_blocksize;
}

void World::set_blocksize(uint16_t blocksize)
{
    m_blocksize = blocksize;
}

void World::set_samplerate(uint32_t samplerate)
{
    m_samplerate = samplerate;
}

void World::parse()
{

}

void World::genreg(const AElement &)
{

}

void World::procreg(const AProcessorElement &)
{

}

void World::alloc(const uint16_t blocksize)
{

}

float**& World::run(const uint16_t nsamples)
{

}

stream* World::mainstream() const
{
    return m_mainstream;
}

vector<pnode*> World::prm_nodes() const
{

}

vector<stream*> World::streams() const
{

}

//---------------------------------------------------------------------------------------------------
// WORLD_INTERFACE
//---------------------------------------------------------------------------------------------------

WorldInterface::WorldInterface() : m_n_inputs(0), m_n_outputs(0), m_output(0)
{
    open(QIODevice::ReadOnly);
}

WorldInterface::~WorldInterface() {}

ON_COMPONENT_COMPLETED ( WorldInterface )
{
    configure();
}

void WorldInterface::configure()
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

void WorldInterface::onAudioStateChanged(QAudio::State state)
{
    qDebug() << state;
}

QQmlListProperty<AElement> WorldInterface::aelements()
{
    return QQmlListProperty<AElement>(this, m_aelements);
}

void WorldInterface::run()
{
    m_output->start( this );
    qDebug() << m_output->error();
}

void WorldInterface::stop()
{
    m_output->stop();
}

qint64 WorldInterface::readData(char* data, qint64 maxlen)
{
    World& w            = World::instance();

    quint16 nout        = m_n_outputs;
    float level         = m_level;
    quint16 bsize       = w.blocksize();
    float* bufdata      = m_buffer;

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

qint64 WorldInterface::writeData(const char* data, qint64 maxlen)
{
    Q_UNUSED ( data )
    Q_UNUSED ( maxlen )

    return 0;
}

qint64 WorldInterface::bytesAvailable()
{
    return 0;
}

uint32_t WorldInterface::samplerate() const
{
    return World::instance().samplerate();
}

uint16_t WorldInterface::blocksize() const
{
    return World::instance().blocksize();
}

uint16_t WorldInterface::n_inputs() const
{
    return m_n_inputs;
}

uint16_t WorldInterface::n_outputs() const
{
    return m_n_outputs;
}

QString WorldInterface::device() const
{
    return m_device;
}

void WorldInterface::set_samplerate(uint32_t samplerate)
{
    World::instance().set_samplerate(samplerate);
}

void WorldInterface::set_blocksize(uint16_t blocksize)
{
    World::instance().set_blocksize(blocksize);
}

void WorldInterface::setn_inputs(uint16_t n_inputs)
{
    m_n_inputs = n_inputs;
}

void WorldInterface::setn_outputs(uint16_t n_outputs)
{
    m_n_outputs = n_outputs;
}

void WorldInterface::set_device(QString device)
{
    m_device = device;
}

//---------------------------------------------------------------------------------------------------
// ...
//---------------------------------------------------------------------------------------------------

#define STRM_DRAIN(target, sz)                              \
    for(int i = 0; i < sz; ++i )                            \
    std::memset(target[i], 0.f, sizeof(float)*BLOCKSIZE);

inline void STRM_POUR (
        float** b1, float** b2, uint16_t nin,
        uint16_t unout, uint16_t uoff, uint16_t nsamples, float b2level)
{
    for(int ch = 0; ch < nin; ++ch)
        if ( ch >= uoff && ch < uoff+unout )
            for( int s = 0; s < nsamples; ++s )
                b1[ch][s] += b2[ch-uoff][s] * b2level;
}

#define STREAM_SOURCE_ALLOC (target,n)                                      \
    target = new float*[n];                                                 \
    for(int i = 0; i < n; ++i)                                              \
    target[i] = (float*) std::calloc(n*BLOCKSIZE, sizeof(float));

#define STREAM_SOURCE_DEALLOC(target, n)                                    \
    for(int i = 0; i < n; ++i)                                              \
    delete target[i];                                                       \
    delete target;

void alloc(const uint16_t nsamples)
{
    vector<pnode*>      prnodes;
    vector<stream*>     strms;
    stream*             mainstream;

    STREAM_SOURCE_ALLOC ( mainstream->src, mainstream->nout );

    for ( const auto& prnode : prnodes )
        STREAM_SOURCE_ALLOC ( prnode->src, prnode->nout );

    for ( const auto& strm : strms )
        STREAM_SOURCE_ALLOC ( strm->src, strm->nout );

}

void parse()
{
    AudioMaster master;
    QList<AudioObject*> direct_children = master.get_units();
}

void unalloc()
{
    vector<pnode*>      prnodes;
    vector<stream*>     strms;
    stream*             mainstream;

    STREAM_SOURCE_DEALLOC ( mainstream->src, mainstream->nout );

    for ( const auto& prnode : prnodes )
        STREAM_SOURCE_DEALLOC ( prnode->src, prnode->nout );

    for ( const auto& strm : strms )
        STREAM_SOURCE_DEALLOC ( strm->src, strm->nout );

}

void run(const uint16_t nsamples)
{
    vector<pnode*>      prnodes;
    vector<stream*>     strms;
    stream*             mainstream;

    // zero out all streams
    for ( const auto& strm : strms )
        STRM_DRAIN ( strm->src, strm->nout );
    STRM_DRAIN ( mainstream->src, mainstream->nout );

    for ( const auto& node : prnodes )
    {
        auto& src   = node->src;
        auto unout  = node->nout;
        auto uoff   = node->off;

        STRM_DRAIN  ( src, unout );
        node->prc   ( src, nsamples );

        for ( const auto &strm : node->streams )
            STRM_POUR ( src, strm->src, unout, uoff );
    }

    for ( const auto& strm : strms )
    {
        auto& src   = strm->src;
        auto unout  = strm->nout;
        auto uoff   = strm->off;

        for ( const auto& pn : strm->pnodes )
        {
            pn->prc ( src, nsamples );
            for ( const auto& substrm : pn->streams )
                STRM_POUR ( src, substrm->src, unout, unin, uoff, nsamples, 1.f);
        }

        STRM_POUR ( src, mainstream->src, unout, uoff );
    }

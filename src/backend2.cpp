#include "backend2.hpp"

WorldStream WorldStream::m_instance = WorldStream();

inline OutStreamNode* outnode_cast(StreamNode const& node)
{
    return qobject_cast<OutStreamNode*>(&node);
}

inline InStreamNode* innode_cast(StreamNode const& node)
{
    return qobject_cast<InStreamNode*>(&node);
}

inline StreamNode* streamnode_cast(QObject* const& qobj)
{
    return qobject_cast<StreamNode*>(qobj);
}

//---------------------------------------------------------------------------------------------------

float OutStreamNode::level() const
{
    return m_level;
}

uint16_t OutStreamNode::offset() const
{
    return m_offset;
}

void OutStreamNode::set_level(float level)
{
    m_level = level;
}

void OutStreamNode::set_offset(uint16_t offset)
{
    m_offset = offset;
}

QQmlListProperty<Fork> OutStreamNode::forks()
{
    return QQmlListProperty<Fork>(this, m_forks);
}

QList<Fork*> OutStreamNode::get_forks() const
{
    return m_forks;
}

uint16_t OutStreamNode::n_outputs() const
{
    return m_n_outputs;
}

void OutStreamNode::setn_outputs(uint16_t n_outputs)
{
    m_n_outputs = n_outputs;
}

//---------------------------------------------------------------------------------------------------
// IN_ELEMENT
//---------------------------------------------------------------------------------------------------

QQmlListProperty<OutStreamNode> InStreamNode::inputs()
{
    return QQmlListProperty<OutStreamNode>(this, m_inputs);
}

uint16_t InStreamNode::n_inputs() const
{
    return m_n_inputs;
}

void InStreamNode::setn_inputs(uint16_t n_inputs)
{
    m_n_inputs = n_inputs;
}

void InStreamNode::add_receive(const Fork &send)
{
    m_receives.push_back(&send);
}

//---------------------------------------------------------------------------------------------------
// IN_OUT_ELEMENT
//---------------------------------------------------------------------------------------------------

bool IOStreamNode::bypassed() const
{
    return m_bypassed;
}

void IOStreamNode::set_bypassed(bool bypassed)
{
    m_bypassed = bypassed;
}

//---------------------------------------------------------------------------------------------------
// FORKS
//---------------------------------------------------------------------------------------------------

Fork::Fork() : m_target(0)
{
    m_emitter = qobject_cast<OutStreamNode*>(parent);
}

Fork::~Fork() {}

ON_COMPONENT_COMPLETED ( Fork )
{
    m_target->add_receive(*this);
}

void Fork::process(float **&, const uint16_t) {}
// nothing to process

bool Fork::prefader() const
{
    return m_prefader;
}

bool Fork::postfx() const
{
    return m_postfx;
}

InStreamNode* const& Fork::to() const
{
    return m_target;
}

OutStreamNode* const& Fork::from() const
{
    return m_emitter;
}

void Fork::set_target(InStreamNode* target)
{
    m_target = target;
}

void Fork::set_postfx(bool postfx)
{
    m_postfx = postfx;
}

void Fork::set_prefader(bool prefader)
{
    m_prefader = prefader;
}

//---------------------------------------------------------------------------------------------------
// STREAMS
//---------------------------------------------------------------------------------------------------

Stream::Stream(const StreamSegment& segment)
{
    m_segments = segment.upstream_segments();
    m_segments.push_back ( segment );
}


Stream::~Stream()
{

}

inline bool Stream::begins_with( const StreamNode& node)
{
    InStreamNode* in  = innode_cast ( node ) ;

    //  if element has multiple inputs
    if ( in && ( in->children().size() > 1 ||
         in->has_receives())) return true;

    else return false;
}

//---------------------------------------------------------------------------------------------------
// SEGMENTS
//---------------------------------------------------------------------------------------------------

StreamSegment::StreamSegment(const StreamNode& outfall) :
    has_downstreams(false),
    has_upstreams(false)
{
    outfall.set_segment  ( *this );
    m_nodes.push_front   ( &outfall );

    if  ( auto out = outnode_cast ( outfall ))
    {
        m_n_outputs = out->n_outputs();
        m_offset = out->offset();
    }

    else
    {
        m_n_outputs = 0;
        m_offset = 0;
    }
}

inline const StreamNode& StreamSegment::upstream_node()
{
    if ( !m_nodes.empty() ) return *m_nodes.first();
}

inline const StreamNode& StreamSegment::downstream_node()
{
    if ( !m_nodes.empty() ) return *m_nodes.last();
}

inline void StreamSegment::zero_pool(const uint16_t bsize)
{
    for ( uint16_t ch = 0; ch < maxchannels; ++ch )
        std::memset(m_pool[ch], 0.f, sizeof(float)*bsize);
}

inline void StreamSegment::alloc_pool(const uint16_t bsize)
{
    m_pool = new float* [ maxchannels ];
    for ( uint16_t ch = 0; ch < maxchannels; ++ch )
        m_pool[ch] = (float*) std::calloc(bsize, sizeof(float));
}

inline void Stream::segpour(StreamSegment const& upseg, StreamSegment const& target)
{


}

inline void Stream::process(const uint16_t nsamples)
{
    for ( StreamSegment* const& seg : m_segments )
    {
        if ( seg->n_inputs() )
            for ( const auto& upseg : seg->upstream_segments())
                segpour(upseg, seg);

        seg->process ( nsamples );
    }
}

inline void StreamSegment::process(const uint16_t nsamples)
{
    for ( StreamNode* const& node : m_nodes )
        node->process ( nsamples );
}

QVector<StreamSegment*> const& StreamSegment::upstream_segments(bool recursive)
{
    QVector<StreamSegment*> segs;

    if ( has_upstreams )
    {
        // we take the upstream-most node
        auto upnode = innode_cast ( upstream_node() );

        // we merge inputs (and receives)
        auto in = upnode->get_inputs();
        for ( const auto& rcv : upnode->receives())
            in += rcv->from ();

        for(const auto& input : in)
        {
            // we push back all inputs' segments if they were not already pulled
            auto segtarget = input->segment();

            if ( !segs.contains ( segtarget ) )
                 segs.push_back ( segtarget );

            if ( recursive )
                segs = segtarget.upstream_segments() + segs;
        }
    }

    return segs;
}

//---------------------------------------------------------------------------------------------------
// STREAM_FACTORY
//---------------------------------------------------------------------------------------------------

StreamMaker::StreamMaker() {}

inline bool has_direct_upstream( const StreamNode& node)
{
    return !node.children().empty();
}

inline bool has_indirect_upstream( const StreamNode& node)
{
    auto in = innode_cast ( node );
    return in && in->has_receives();
}

void StreamMaker::parse_upstream(const StreamSegment& segment)
{
    StreamNode& target = segment.upstream();

    if  ( !has_direct_upstream ( target) )
    {
        // segment is complete, no further parsing required
        segment.has_upstreams = has_indirect_upstream ( target );
        m_segments.push_back ( &segment );
        return;
    }
    else if ( segment.has_upstreams = Stream::begins_with ( target ))
    {
        // segment is complete, but there are others upstream
        segment.has_downstreams = true;
        m_segments.push_back ( &segment );

        // continue parsing
        for ( const auto& child : target.children() )
            parse_upstream ( *streamnode_cast ( child ) );
    }
    else
    {
        for ( const auto& child : target.children() )
        {
            // segment is not complete yet

            // we add child to the segment as the new first node
            // and continue to parse upstream...
            segment.push_front ( *streamnode_cast ( child ) );
            parse_upstream ( segment );
        }
    }
}

inline void StreamMaker::parse_upstream(const StreamNode& outfall)
{
    auto segment = new StreamSegment( outfall );
    segment->has_downstreams = outnode_cast ( outfall );

    parse_upstream( *segment );

    // note a stream's outfall is not necessarily worldstream's pool
    // it can be an analyzer, or a graphic element
}

inline void StreamMaker::resolve_streams(StreamNode const& node)
{
    // we build streams from firsts world segments
    // this will recursively parse all segments upstream
    auto stream = new Stream ( node.segment() );

    m_streams.push_back ( stream );

}

QVector<Stream*> StreamMaker::streams() const
{
    return m_streams;
}

//---------------------------------------------------------------------------------------------------
// WORLD
//---------------------------------------------------------------------------------------------------

WorldStream& WorldStream::instance()
{
    return m_singleton;
}

WorldStream::WorldStream() : m_n_inputs(0), m_n_outputs(0), m_output(0)
{
    open(QIODevice::ReadOnly);
}

WorldStream::~WorldStream() {}

ON_COMPONENT_COMPLETED ( WorldStream )
{
    configure();

    StreamMaker factory;
    factory.upstream ( *this );
}

void WorldStream::configure()
{
    m_format.setCodec           ( "audio/pcm" );
    m_format.setByteOrder       ( QAudioFormat::LittleEndian );
    m_format.setSampleType      ( QAudioFormat::SignedInt );
    m_format.setSampleSize      ( 16 );
    m_format.setSampleRate      ( m_samplerate );
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

inline void WorldStream::onAudioStateChanged(QAudio::State state)
{
    qDebug() << state;
}

QQmlListProperty<StreamNode> WorldStream::nodes()
{
    return QQmlListProperty<StreamNode>(this, m_nodes);
}

inline void WorldStream::process(const uint16_t nsamples)
{
    for ( const auto& stream : m_streams )
        stream->process ( nsamples );
}

void WorldStream::run()
{
    m_output  -> start( this );
    qDebug()  << m_output->error();
}

void WorldStream::stop()
{
    m_output->stop();
}

qint64 WorldStream::readData(char* data, qint64 maxlen)
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

qint64 WorldStream::writeData(const char* data, qint64 maxlen)
{
    Q_UNUSED ( data )
    Q_UNUSED ( maxlen )

    return 0;
}

qint64 WorldStream::bytesAvailable()
{
    return 0;
}

uint32_t WorldStream::samplerate() const
{
    return m_samplerate;
}

uint16_t WorldStream::blocksize() const
{
    return m_blocksize;
}

QString WorldStream::device() const
{
    return m_device;
}

void WorldStream::set_samplerate(uint32_t samplerate)
{
    m_samplerate = samplerate;
}

void WorldStream::set_blocksize(uint16_t blocksize)
{
    m_blocksize = blocksize;
}

void WorldStream::set_device(QString device)
{
    m_device = device;
}

void WorldStream::get_nodes() const
{
    return m_nodes;
}

#include "audio.hpp"
#include <QtDebug>
#include <qendian.h>
#include <cmath>
#include <src/oscquery/node.hpp>
#include <memory>
#include <QtGlobal>

static const QStringList g_ignore = {
    "parentStream", "subnodes", "exposeDevice", "objectName", "exposePath"
};

static const QStringList g_stream =
{
    "active", "mute", "numInputs", "numOutputs", "parentChannels", "level", "dBlevel"
};

StreamNode::StreamNode() : m_level(1.0), m_db_level(0.0),
    m_num_inputs(0), m_num_outputs(0), m_max_outputs(0), m_parent_channels(0),
    m_mute(false), m_active(true),
    m_in(nullptr), m_out(nullptr),
    m_exp_device(nullptr), m_parent_stream(nullptr)
{
    m_stream_properties.block_size      = 0;
    m_stream_properties.sample_rate     = 0;
}

StreamNode::~StreamNode()
{
    StreamNode::deleteBuffer( m_in, m_num_inputs, m_stream_properties.block_size );
    StreamNode::deleteBuffer( m_out, m_num_outputs, m_stream_properties.block_size );

    for ( const auto& subnode : m_subnodes )
        delete subnode;
}

void StreamNode::componentComplete()
{

}

void StreamNode::setNumInputs(uint16_t num_inputs)
{
    if ( m_num_inputs != num_inputs )
    {
        m_num_inputs = num_inputs;
        emit numInputsChanged();
    }
}

void StreamNode::setNumOutputs(uint16_t num_outputs)
{    
    if ( m_num_outputs != num_outputs )
    {
        m_num_outputs = num_outputs;

        QVariantList list;

        for ( quint16 ch = 0; ch < num_outputs; ++ch)
        {
           list.push_back(QVariant(ch));
            m_parent_channels = list;
        }

        if ( m_subnodes.isEmpty() ) setMaxOutputs(num_outputs);
        emit numOutputsChanged();
    }
}

void StreamNode::setMaxOutputs(uint16_t max_outputs)
{
    m_max_outputs = max_outputs;

    QVariantList list;
    for ( quint16 i = 0; i < m_max_outputs; ++i ) list << i;

    m_parent_channels = list;
}

void StreamNode::setMute(bool mute)
{
    if ( mute != m_mute )
    {
        m_mute = mute;
        emit muteChanged();
    }
}

void StreamNode::setActive(bool active)
{
    if ( active != m_active )
    {
        m_active = active;
        emit activeChanged();
    }
}

void StreamNode::setLevel(qreal level)
{
    if ( level != m_level ) emit levelChanged();
    m_level = level;
    m_db_level = std::log10(level)*(qreal)20.f;
}

void StreamNode::setDBlevel(qreal db)
{
    m_db_level = db;
    m_level = std::pow(10.f, db*.05);
}

void StreamNode::setExposePath(QString path)
{
    m_exp_path = path;

    if ( m_exp_device )
         m_exp_node = m_exp_device->findOrCreateNode(path);

    else if ( auto dev = WPNDevice::instance() )
    {
        m_exp_device = dev;
        m_exp_node = m_exp_device->findOrCreateNode(path);
    }

    else return;

    auto pcount = metaObject()->propertyCount();

    auto stream     = m_exp_node->createSubnode("stream");
    auto parameters = m_exp_node->createSubnode("properties");

    for ( quint16 i = 0; i < pcount; ++i )
    {
        auto property = metaObject()->property(i);
        QString name = property.name();

        WPNNode* node;

        if      ( g_ignore.contains(name) ) continue;
        else if ( g_stream.contains(name) )
             node = stream->createSubnode(property.name());
        else node = parameters->createSubnode(property.name());

        node->setTarget ( this, property );
    }

    expose(m_exp_node);
}

void StreamNode::setExposeDevice(WPNDevice* device)
{
    m_exp_device = device;
}

void StreamNode::setParentStream(StreamNode* stream)
{
    m_parent_stream = stream;
    m_parent_stream->appendSubnode(this);
}

//-------------------------------------------------------------------------------------------

QVector<quint16> StreamNode::parentChannelsVec() const
{
    QVector<quint16> res;

    if ( m_parent_channels.type() == QMetaType::QVariantList )
    {
        for ( const auto& ch : m_parent_channels.toList() )
            res << ch.toInt();
    }

    else if ( m_parent_channels.type() == QMetaType::Int )
        res << m_parent_channels.toInt();

    return res;

}

void StreamNode::setParentChannels(QVariant pch)
{
    m_parent_channels = pch;
}

QQmlListProperty<StreamNode> StreamNode::subnodes()
{
    return QQmlListProperty<StreamNode>( this, this,
                           &StreamNode::appendSubnode,
                           &StreamNode::subnodesCount,
                           &StreamNode::subnode,
                           &StreamNode::clearSubnodes );
}

StreamNode* StreamNode::subnode(int index) const
{
    return m_subnodes.at(index);
}

void StreamNode::appendSubnode(StreamNode* subnode)
{
    m_subnodes.append(subnode);
    if ( !m_num_inputs ) setMaxOutputs(subnode->maxOutputs());
}

int StreamNode::subnodesCount() const
{
    return m_subnodes.count();
}

void StreamNode::clearSubnodes()
{
    m_subnodes.clear();
}

// statics --

void StreamNode::appendSubnode(QQmlListProperty<StreamNode>* list, StreamNode* subnode)
{
    reinterpret_cast<StreamNode*>(list->data)->appendSubnode(subnode);
}

void StreamNode::clearSubnodes(QQmlListProperty<StreamNode>* list )
{
    reinterpret_cast<StreamNode*>(list->data)->clearSubnodes();
}

StreamNode* StreamNode::subnode(QQmlListProperty<StreamNode>* list, int i)
{
    return reinterpret_cast<StreamNode*>(list->data)->subnode(i);
}

int StreamNode::subnodesCount(QQmlListProperty<StreamNode>* list)
{
    return reinterpret_cast<StreamNode*>(list->data)->subnodesCount();
}

//-------------------------------------------------------------------------------------------

void StreamNode::allocateBuffer(float**& buffer, quint16 nchannels, quint16 nsamples )
{
    buffer = new float* [ nchannels ]();
    for ( uint16_t ch = 0; ch < nchannels; ++ch )
        buffer[ch] = new float [ nsamples ]();
}

void StreamNode::deleteBuffer(float**& buffer, quint16 nchannels, quint16 nsamples )
{
    if ( ! buffer ) return;

    for ( uint16_t ch = 0; ch < nchannels; ++ch )
        delete [] buffer[ch];

    delete [] buffer;
}

void StreamNode::resetBuffer(float**& buffer, quint16 nchannels, quint16 nsamples )
{
    for ( uint16_t ch = 0; ch < nchannels; ++ch )
        memset(buffer[ch], 0.f, sizeof(float)*nsamples);
}

void StreamNode::applyGain(float**& buffer, quint16 nchannels, quint16 nsamples, float gain)
{
    if ( gain == 1.f ) return;

    for ( quint16 ch = 0; ch < nchannels; ++ch )
        for ( quint16 s = 0; s < nsamples; ++s )
            buffer[ch][s] *= gain;
}

void StreamNode::mergeBuffers(float**& lhs, float** rhs, quint16 lnchannels,
                              quint16 rnchannels, quint16 nsamples )
{
    for ( quint16 ch = 0; ch < rnchannels; ++ch )
        for ( quint16 s = 0; s < nsamples; ++s )
            lhs[ch][s] += rhs[ch][s];
}

void StreamNode::preinitialize(StreamProperties properties)
{
    m_stream_properties = properties;

    if ( m_stream_properties.block_size != properties.block_size && m_out )
    {
        StreamNode::deleteBuffer(m_in, m_num_inputs, m_stream_properties.block_size );
        StreamNode::deleteBuffer(m_out, m_num_outputs, m_stream_properties.block_size);
    }

    StreamNode::allocateBuffer(m_in, m_num_inputs, properties.block_size);
    StreamNode::allocateBuffer(m_out, m_num_outputs, properties.block_size);

    initialize(properties.block_size);

    for ( const auto& subnode : m_subnodes )
        subnode->preinitialize(properties);
}

float** StreamNode::preprocess(float** buf, qint64 le)
{   
    if ( !m_num_inputs ) // if generator, pass the buffer down the chain
    {
        float** ubuf = process(buf, le);
        StreamNode::applyGain(ubuf, m_num_outputs, le, m_level);

        for ( const auto& subnode : m_subnodes )
            if ( subnode->active() && subnode->numInputs() == m_num_outputs )
                ubuf = subnode->preprocess(ubuf, le);

        return ubuf;
    }
    else
    {
        // otherwise mix all sources down to an array of channels       
        float** in = m_in;
        StreamNode::resetBuffer(in, m_num_inputs, le);

        if  ( buf != nullptr )
            StreamNode::mergeBuffers(in, buf, m_num_inputs, m_num_inputs, le);

        for ( const auto& subnode : m_subnodes )
        {
            if ( subnode->active() )
            {
                auto pch     = subnode->parentChannelsVec();
                auto genbuf  = subnode->preprocess(nullptr, le);

                for ( quint16 ch = 0; ch < pch.size(); ++ch )
                    for ( quint16 s = 0; s < le; ++s )
                        in[pch[ch]][s] += genbuf[ch][s];
            }
        }

        return process(in, le);
    }
}

//-----------------------------------------------------------------------------------------------

WorldStream::WorldStream() : m_sample_rate(44100), m_block_size(512)
{

}

WorldStream::~WorldStream()
{
    m_stream_thread.terminate();
    delete m_stream;
}

void WorldStream::setSampleRate(uint32_t sample_rate)
{
    if ( sample_rate != m_sample_rate ) emit sampleRateChanged();
    m_sample_rate = sample_rate;
}

void WorldStream::setBlockSize(uint16_t block_size)
{
    if ( block_size != m_block_size ) emit blockSizeChanged();
    m_block_size = block_size;
}

void WorldStream::setInDevice(QString device)
{
    if ( device != m_in_device ) emit inDeviceChanged();
    m_in_device = device;
}

void WorldStream::setOutDevice(QString device)
{
    if ( device != m_out_device ) emit outDeviceChanged();
    m_out_device = device;
}

void WorldStream::componentComplete()
{
    QAudioFormat format;

    format.setCodec           ( "audio/pcm" );
    format.setByteOrder       ( QAudioFormat::LittleEndian );
    format.setSampleType      ( QAudioFormat::Float );
    format.setSampleSize      ( 32 );
    format.setSampleRate      ( m_sample_rate );
    format.setChannelCount    ( m_num_outputs );

    auto device_info = QAudioDeviceInfo::defaultOutputDevice();

    if ( !m_out_device.isEmpty() || m_out_device != "default" )
    {
        auto devices = device_info.availableDevices(QAudio::AudioOutput);
        for ( const auto& device : devices )
        {
            if ( device.deviceName() == m_out_device )
            {
                device_info = device;
                break;
            }
        }
    }

    if ( !device_info.isFormatSupported(format) )
        qDebug() << "[AUDIO] Format not supported by chosen device";

    m_stream = new AudioStream(*this, format, device_info);
    m_stream->moveToThread  ( &m_stream_thread );

    QObject::connect(this, &WorldStream::startStream, m_stream, &AudioStream::start);
    QObject::connect(this, &WorldStream::stopStream, m_stream, &AudioStream::stop);
    QObject::connect(this, &WorldStream::configure, m_stream, &AudioStream::configure);

    emit configure();

    m_stream_thread.start   ( QThread::TimeCriticalPriority );
}

void WorldStream::start()
{
    emit startStream();
}

void WorldStream::stop()
{
    emit stopStream();
}

// -----------------------------------------------------------------------------------------

AudioStream::AudioStream(const WorldStream& world, QAudioFormat format, QAudioDeviceInfo device_info) :
    m_world(world), m_format(format), m_device_info(device_info)
{
}

AudioStream::~AudioStream()
{
    m_output->stop();

    close();
    delete m_input;
    delete m_output;
    delete m_pool;
}

void AudioStream::configure()
{
    m_output = new QAudioOutput(m_device_info, m_format);

    QObject::connect( m_output, &QAudioOutput::stateChanged,
                      this, &AudioStream::onAudioStateChanged);
}

void AudioStream::start()
{
    for ( const auto& input : m_world.m_subnodes )
        input->preinitialize({ m_world.m_sample_rate, m_world.m_block_size });

    StreamNode::allocateBuffer(m_pool, m_world.m_num_outputs, m_world.m_block_size);
    m_output->setBufferSize(m_world.m_block_size*m_world.numOutputs()*sizeof(float));

    open(QIODevice::ReadOnly);
    m_output->start(this);

    qDebug() << "AudioStream buffer size initialized at"
             << m_output->bufferSize() << "bytes";
}

void AudioStream::stop()
{
    m_output->stop();
}

void AudioStream::onAudioStateChanged(QAudio::State state)
{
    auto obj = qobject_cast<QAudioOutput*>(QObject::sender());
    qDebug() << "[AUDIO]" << state;

    if ( obj->error() == QAudio::UnderrunError )
    {
        qDebug() << "QAudio::UnderrunError";

        // restart stream
        stop();
        start();
    }
}

qint64 AudioStream::readData(char* data, qint64 maxlen)
{
    auto inputs     = m_world.m_subnodes;
    quint16 nout    = m_world.m_num_outputs;
    quint16 bsize   = m_world.m_block_size;
    qreal level     = m_world.m_level;
    float** buf     = m_pool;

    StreamNode::resetBuffer(m_pool, nout, bsize);

    for ( const auto& input : inputs )
    {
        if ( !input->active() ) continue;       

        float** cdata   = input->preprocess ( nullptr, bsize );
        auto pch        = input->parentChannelsVec();

        if ( pch.size() > nout ) pch.resize(nout);

        for ( quint16 s = 0; s < bsize; ++s )
            for ( quint16 ch = 0; ch < pch.size(); ++ch )
                buf[pch[ch]][s] += ( cdata[ch][s] *level );
    }

        for ( quint16 s = 0; s < bsize; ++s )
        {
            for ( quint16 ch = 0; ch < nout; ++ch )
            {
                // convert to interleaved little endian
                qToLittleEndian<float>(buf[ch][s], data);
                data += sizeof(float);
            }
        }

    // i.e. block_size * 4bytes per value * numChannels
    return ( bsize*sizeof(float)*nout );

}

qint64 AudioStream::writeData(const char* data, qint64 len)
{
    Q_UNUSED ( data );
    Q_UNUSED ( len );

    return 0;
}

qint64 AudioStream::bytesAvailable() const
{
    return 0;
}

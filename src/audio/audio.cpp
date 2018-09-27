#include "audio.hpp"
#include <QtDebug>
#include <qendian.h>
#include <cmath>

void StreamNode::setNumInputs(uint16_t num_inputs)
{
    if ( m_num_inputs != num_inputs ) emit numInputsChanged();
    m_num_inputs = num_inputs;
}

void StreamNode::setNumOutputs(uint16_t num_outputs)
{
    if ( m_num_outputs != num_outputs ) emit numOutputsChanged();
    m_num_outputs = num_outputs;
}

void StreamNode::setMute(bool mute)
{
    if ( mute != m_mute ) emit muteChanged();
    m_mute = mute;
}

void StreamNode::setActive(bool active)
{
    if ( active != m_active ) emit activeChanged();
    m_active = active;
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

inline void StreamNode::allocateBuffer(float**& buffer, quint16 nchannels, quint16 nsamples )
{
    buffer = new float* [ nchannels ];
    for ( uint16_t ch = 0; ch < nchannels; ++ch )
        buffer[ch] = (float*) std::calloc(nsamples, sizeof(float));
}

inline void StreamNode::resetBuffer(float**& buffer, quint16 nchannels, quint16 nsamples )
{
    for ( uint16_t ch = 0; ch < nchannels; ++ch )
        std::memset(buffer[ch], 0.f, sizeof(float)*nsamples);
}

//-----------------------------------------------------------------------------------------------

QQmlListProperty<OutStreamNode>InStreamNode::inputs()
{
    return QQmlListProperty<OutStreamNode>(this, m_inputs);
}

const QList<OutStreamNode*>& InStreamNode::getInputs() const
{
    return m_inputs;
}

void InStreamNode::initialize(StreamProperties properties)
{
    m_stream_properties = properties;
    StreamNode::allocateBuffer(m_in, m_num_inputs, properties.block_size);

    userInitialize(properties.block_size);
}

float** InStreamNode::process(float** buf, qint64 le)
{
    return nullptr;
}

//-----------------------------------------------------------------------------------------------

QQmlListProperty<InStreamNode> OutStreamNode::outputs()
{
    return QQmlListProperty<InStreamNode>(this, m_outputs);
}

const QList<InStreamNode*>& OutStreamNode::getOutputs() const
{
    return m_outputs;
}

void OutStreamNode::setParentChannels(QVector<int> pch)
{
    m_pch = pch;
}

void OutStreamNode::setNumOutputs(uint16_t nout)
{
    m_pch = QVector<int>{nout-1};
    m_num_outputs = nout;
}

void OutStreamNode::initialize(StreamProperties properties)
{
    m_stream_properties = properties;
    StreamNode::allocateBuffer(m_out, m_num_outputs, properties.block_size);

    userInitialize(properties.block_size);
}

float** OutStreamNode::process(float** buf, qint64 le)
{
    // process generator unit, pass it to effects
    float** ubuf = userProcess(nullptr, le);
    return ubuf;
}

//-----------------------------------------------------------------------------------------------

QQmlListProperty<OutStreamNode>IOStreamNode::inputs()
{
    return QQmlListProperty<OutStreamNode>(this, m_inputs);
}

const QList<OutStreamNode*>& IOStreamNode::getInputs() const
{
    return m_inputs;
}

QQmlListProperty<InStreamNode> IOStreamNode::outputs()
{
    return QQmlListProperty<InStreamNode>(this, m_outputs);
}

const QList<InStreamNode*>& IOStreamNode::getOutputs() const
{
    return m_outputs;
}

void IOStreamNode::setParentChannels(QVector<int> pch)
{
    m_pch = pch;
}

void IOStreamNode::initialize(StreamProperties properties)
{
    m_stream_properties = properties;
    StreamNode::allocateBuffer(m_in, m_num_inputs, properties.block_size);
    StreamNode::allocateBuffer(m_out, m_num_outputs, properties.block_size);

    userInitialize(properties.block_size);
}

float** IOStreamNode::process(float** buf, qint64 le)
{
    return buf;
}

//-----------------------------------------------------------------------------------------------

WorldStream::WorldStream() : m_sample_rate(44100), m_block_size(512),
    m_input(nullptr), m_output(nullptr), m_level(1.0)
{

}

void WorldStream::setNumInputs(uint16_t num_inputs)
{
    if ( m_num_inputs != num_inputs ) emit numInputsChanged();
    m_num_inputs = num_inputs;
}

void WorldStream::setNumOutputs(uint16_t num_outputs)
{
    if ( m_num_outputs != num_outputs ) emit numOutputsChanged();
    m_num_outputs = num_outputs;
}

void WorldStream::setMute(bool mute)
{
    if ( mute != m_mute ) emit muteChanged();
    m_mute = mute;
}

void WorldStream::setActive(bool active)
{
    if ( active != m_active ) emit activeChanged();
    m_active = active;
}

void WorldStream::setLevel(qreal level)
{
    if ( level != m_level ) emit levelChanged();
    m_level = level;
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

QQmlListProperty<OutStreamNode>WorldStream::inputs()
{
    return QQmlListProperty<OutStreamNode>(this, this,
                                           &WorldStream::appendInput,
                                           &WorldStream::inputCount,
                                           &WorldStream::input,
                                           &WorldStream::clearInputs);
}

OutStreamNode* WorldStream::input(int index) const
{
    return m_inputs.at(index);
}

void WorldStream::appendInput(OutStreamNode* input)
{
    m_inputs.append(input);
}

int WorldStream::inputCount() const
{
    return m_inputs.count();
}

void WorldStream::clearInputs()
{
    m_inputs.clear();
}

// statics --

void WorldStream::appendInput(QQmlListProperty<OutStreamNode>* list, OutStreamNode* input)
{
    reinterpret_cast<WorldStream*>(list->data)->appendInput(input);
}

void WorldStream::clearInputs(QQmlListProperty<OutStreamNode>* list )
{
    reinterpret_cast<WorldStream*>(list->data)->clearInputs();
}

OutStreamNode* WorldStream::input(QQmlListProperty<OutStreamNode>* list, int i)
{
    return reinterpret_cast<WorldStream*>(list->data)->input(i);
}

int WorldStream::inputCount(QQmlListProperty<OutStreamNode>* list)
{
    return reinterpret_cast<WorldStream*>(list->data)->inputCount();
}

void WorldStream::componentComplete()
{
    m_format.setCodec           ( "audio/pcm" );
    m_format.setByteOrder       ( QAudioFormat::LittleEndian );
    m_format.setSampleType      ( QAudioFormat::SignedInt );
    m_format.setSampleSize      ( 16 );
    m_format.setSampleRate      ( m_sample_rate );
    m_format.setChannelCount    ( m_num_outputs );

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

    if ( !device_info.isFormatSupported(m_format) )
        qDebug() << "[AUDIO] Format not supported";

    m_output = new QAudioOutput(device_info, m_format);

    QObject::connect ( m_output, SIGNAL(stateChanged(QAudio::State)),
                      this, SLOT(onAudioStateChanged(QAudio::State)));
}

void WorldStream::onAudioStateChanged(QAudio::State state) const
{
    qDebug() << "[AUDIO]" << state;
}

void WorldStream::start()
{
    for ( const auto& input : m_inputs )
        input->initialize({m_sample_rate, m_block_size});

    StreamNode::allocateBuffer(m_pool, m_num_outputs, m_block_size);

    open(QIODevice::ReadOnly);
    m_output->start(this);
}

void WorldStream::stop()
{
    m_output->stop();
}

qint64 WorldStream::readData(char* data, qint64 maxlen)
{
    auto inputs     = m_inputs;
    quint16 nout    = m_num_outputs;
    quint16 bsize   = m_block_size;
    float** buf     = m_pool;
    qreal level     = m_level;

    StreamNode::resetBuffer(m_pool, nout, bsize);

    for ( const auto& input : inputs )
    {
        float** cdata   = input->process ( nullptr, bsize );
        auto pch        = input->parentChannels();

        for ( quint16 s = 0; s < bsize; ++s )
            for ( quint16 ch = 0; ch < pch.size(); ++ch )
                buf[pch[ch]][s] += ( cdata[ch][s] *level );
    }


        for ( quint16 s = 0; s < bsize; ++s )
        {
            for ( quint16 ch = 0; ch < nout; ++ch )
            {
                // convert to interleaved little endian int16
                qint16 sdata = static_cast<qint16>(buf[ch][s] * 32767);
                qToLittleEndian<qint16>(sdata, data);
                data += 2;
            }
        }


    // i.e. block_size * 2bytes per value * numChannels
    return ( bsize*2*nout );

}

qint64 WorldStream::writeData(const char* data, qint64 len)
{
    Q_UNUSED ( data );
    Q_UNUSED ( len );

    return 0;

}

qint64 WorldStream::bytesAvailable() const
{
    return 0;
}

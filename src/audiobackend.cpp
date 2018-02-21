#include "audiobackend.hpp"

quint32  AudioBackend::m_sample_rate;
quint16  AudioBackend::m_block_size;
quint16  AudioBackend::m_num_inputs;
quint16  AudioBackend::m_num_outputs;

// AUDIO_SEND ----------------------------------------------------------------------------

AudioSend::AudioSend() : m_target(0) {}
AudioSend::~AudioSend() {}

void AudioSend::classBegin() {}
void AudioSend::componentComplete()
{
    if( m_target ) m_target->addReceive(*this);
}

AudioEffectObject* AudioSend::target() const
{
    return m_target;
}

float AudioSend::level() const
{
    return m_level;
}

bool AudioSend::prefader() const
{
    return m_prefader;
}

int AudioSend::offset() const
{
    return m_offset;
}

bool AudioSend::active() const
{
    return m_active;
}

bool AudioSend::muted() const
{
    return m_muted;
}

void AudioSend::setTarget(AudioEffectObject *obj)
{
    m_target = obj;
}

void AudioSend::setLevel(const float level)
{
    m_level = level;
}

void AudioSend::setPrefader(const bool prefader)
{
    m_prefader = prefader;
}

void AudioSend::setOffset(const int offset)
{
    m_offset = offset;
}

void AudioSend::setActive(const bool active)
{
    m_active = active;
}

void AudioSend::setMuted(const bool muted)
{
    m_muted = muted;
}

// AUDIO_OBJECT --------------------------------------------------------------------------

AudioObject::~AudioObject()
{
    IODEALLOC( m_outputs, m_num_outputs );
}

QQmlListProperty<AudioSend> AudioObject::sends()
{
    return QQmlListProperty<AudioSend>(this, m_sends);
}

float AudioObject::level() const
{
    return m_level;
}

quint16 AudioObject::offset() const
{
    return m_offset;
}

quint16 AudioObject::numOutputs() const
{
    return m_num_outputs;
}

bool AudioObject::active() const
{
    return m_active;
}

bool AudioObject::muted() const
{
    return m_muted;
}

void AudioObject::setActive(const bool active)
{
    m_active = active;
    emit activeChanged();
}

void AudioObject::setMuted(const bool muted)
{
    m_muted = muted;
    emit mutedChanged();
}

void AudioObject::setLevel(const float level)
{
    m_level = level;
}

void AudioObject::setOffset(const quint16 offset)
{
    m_offset = offset;
}

void AudioObject::setNumOutputs(const quint16 num_outputs)
{
    m_num_outputs = num_outputs;
    emit numOutputsChanged();
}

// AUDIO_EFFECT_OBJECT --------------------------------------------------------

AudioEffectObject::~AudioEffectObject()
{
    IODEALLOC( m_inbuf, m_num_inputs );
}

QQmlListProperty<AudioObject> AudioEffectObject::inputs()
{
    return QQmlListProperty<AudioObject>(this, m_inputs);
}

QQmlListProperty<AudioSend> AudioEffectObject::receives()
{
    return QQmlListProperty<AudioSend>(this, m_receives);
}

quint16 AudioEffectObject::numInputs() const
{
    return m_num_inputs;
}

void AudioEffectObject::setNumInputs(const quint16 num_inputs)
{
    m_num_inputs = num_inputs;
    emit numInputsChanged();
}

void AudioEffectObject::addReceive(AudioSend &receive)
{
    m_receives.push_back(&receive);
}

float**& AudioEffectObject::get_inputs(const quint64 nsamples)
{
    auto inputs     = m_inputs;
    auto nin        = m_num_inputs;
    float** in      = IN;

    for ( const auto& input : inputs )
    {
        if ( input->active() )
        {
            uint16_t unout  = input->numOutputs();
            uint16_t uoff   = input->offset();
            float** buf     = input->process(nsamples);
            inbufmerge      ( in, buf, nin, unout, uoff, nsamples, 1.f );
        }
    }

    for ( const auto& rcv : m_receives )
    {
        auto sender = dynamic_cast<AudioObject*>(rcv->parent());
        if ( sender->active() && rcv->active() )
        {
            uint16_t unout  = sender->numOutputs();
            uint16_t uoff   = sender->offset();
            float** buf     = sender->process(nsamples);
            inbufmerge      ( in, buf, nin, unout, uoff, nsamples, rcv->level() );
        }
    }

    return IN;
}

// AUDIO_BACKEND --------------------------------------------------------------

AudioBackend::AudioBackend() :

    // default property values
    m_muted(false),
    m_active(false),
    m_output(0)
{
    m_num_inputs    = 0;
    m_num_outputs   = 2;
    m_sample_rate   = 44100;
    m_block_size    = 512;
}

void AudioBackend::classBegin() {}
void AudioBackend::componentComplete()
{
    configure();
}

void AudioBackend::configure()
{
    m_format.setCodec           ( "audio/pcm" );
    m_format.setByteOrder       ( QAudioFormat::LittleEndian );
    m_format.setSampleType      ( QAudioFormat::SignedInt );
    m_format.setSampleSize      ( 16 );
    m_format.setSampleRate      ( m_sample_rate );
    m_format.setChannelCount    ( m_num_outputs );

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

void AudioBackend::onAudioStateChanged(QAudio::State state)
{
    qDebug() << state;
}

QQmlListProperty<AudioMaster> AudioBackend::masters()
{
    return QQmlListProperty<AudioMaster>(this, m_masters);
}

void AudioBackend::start()
{
    auto master = m_masters[0];
    if ( ! master )
    {
        qDebug() << "Couldn't find QIODevice child";
        return;
    }

    m_output->start(master);
    qDebug() << m_output->error();
}

void AudioBackend::stop()
{

}

void AudioBackend::suspend()
{
    m_output->suspend();
}

void AudioBackend::resume()
{
    m_output->resume();
}

QString AudioBackend::device() const
{
    return m_device;
}

quint16 AudioBackend::numInputs()
{
    return AudioBackend::m_num_inputs;
}

quint16 AudioBackend::numOutputs()
{
    return AudioBackend::m_num_outputs;
}

quint32 AudioBackend::sampleRate()
{
    return AudioBackend::m_sample_rate;
}

quint16 AudioBackend::blockSize()
{
    return AudioBackend::m_block_size;
}

bool AudioBackend::active() const
{
    return m_active;
}

bool AudioBackend::muted() const
{
    return m_muted;
}

void AudioBackend::setDevice(const QString &device)
{
    m_device = device;
}

void AudioBackend::setNumInputs(const quint16 num_inputs)
{
    AudioBackend::m_num_inputs = num_inputs;
}

void AudioBackend::setNumOutputs(const quint16 num_outputs)
{
    AudioBackend::m_num_outputs = num_outputs;
}

void AudioBackend::setSampleRate(quint32 sample_rate)
{
    AudioBackend::m_sample_rate = sample_rate;
}

void AudioBackend::setBlockSize(const quint16 block_size)
{
    AudioBackend::m_block_size = block_size;
}

void AudioBackend::setActive(const bool active)
{
    m_active = active;
}

void AudioBackend::setMuted(const bool muted)
{
    m_muted = muted;
}





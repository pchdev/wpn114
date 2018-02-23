#include "audiomaster.hpp"
#include <qendian.h>

#define BSIZE AudioBackend::blockSize()
#define BACKEND_NOUT AudioBackend::numOutputs()

AudioMaster::AudioMaster() : m_active(true), m_muted(false), m_level(1.f), m_buf(0),
    m_num_inputs(0), m_num_outputs(BACKEND_NOUT)
{
    open(QIODevice::ReadOnly);

    // prepare zeroed interleaved buffer
    m_buf = (float*) std::calloc(BSIZE * m_num_outputs, sizeof(float));
}

AudioMaster::~AudioMaster()
{
    delete m_buf;
}

QQmlListProperty<AudioObject> AudioMaster::units()
{
    return QQmlListProperty<AudioObject>(this, m_units);
}

quint16 AudioMaster::numInputs() const
{
    return m_num_inputs;
}

quint16 AudioMaster::numOutputs() const
{
    return m_num_outputs;
}

float AudioMaster::level() const
{
    return m_level;
}

// muted should be the only one to use
bool AudioMaster::active() const
{
    return m_active;
}

bool AudioMaster::muted() const
{
    return m_muted;
}

void AudioMaster::setLevel(const float level)
{
    m_level = level;
}

void AudioMaster::setActive(const bool active)
{
    m_active = active;
}

void AudioMaster::setMuted(const bool muted)
{
    m_muted = muted;
}

void AudioMaster::setNumInputs(const quint16 num_inputs)
{
    m_num_inputs = num_inputs;
}

void AudioMaster::setNumOutputs(const quint16 num_outputs)
{
    m_num_outputs = num_outputs;
}

qint64 AudioMaster::readData(char *data, qint64 maxlen)
{
    auto units          = m_units;
    quint16 nout        = m_num_outputs;
    quint16 bsize       = BSIZE;
    float level         = m_level;
    float* bufdata      = m_buf;

    // zero out buffer
    std::memset(bufdata, 0.f, sizeof(float)*bsize*nout);

    for ( const auto& unit : units )
    {
        if ( unit->active() )
        {
            float** cdata = unit->process(bsize);
            quint16 unout = unit->numOutputs();
            quint16 uoff  = unit->offset();

            for ( quint16 s = 0; s < bsize; ++s )
                for ( quint16 ch = 0; ch < nout; ++ch )
                {
                    if ( ch >= uoff && ch < uoff+unout )
                        *bufdata += cdata[ch-uoff][s] * level;
                    bufdata++;
                }
            // reset pointer when done
            bufdata -= bsize*nout;
        }
    }

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

qint64 AudioMaster::writeData(const char *data, qint64 len)
{
    Q_UNUSED    ( data );
    Q_UNUSED    ( len );
    return      0;
}

qint64 AudioMaster::bytesAvailable() const
{
    return 0;
}

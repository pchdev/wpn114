#include "soundfile.hpp"
#include <QtDebug>
#include <qendian.h>

Soundfile::Soundfile() : m_file(nullptr), m_buffer(nullptr)
{

}

Soundfile::Soundfile(QString path) : m_file(new QFile(path)), m_buffer(nullptr)
{
    setPath(path);
}

void Soundfile::setPath(QString path)
{
    if ( !m_file->open(QIODevice::ReadOnly) )
    {
        qDebug() << m_file->errorString();
        return;
    }

    m_stream = new QDataStream(m_file);
    if ( m_path.endsWith(".wav") ) metadataWav();
}

void Soundfile::metadataWav()
{
    WavMetadata data;

    m_stream->setByteOrder(QDataStream::BigEndian);
    *m_stream >> data.chunk_id;

    m_stream->setByteOrder(QDataStream::LittleEndian);
    *m_stream >> data.chunk_size;

    m_stream->setByteOrder(QDataStream::BigEndian);
    *m_stream >> data.format;

    m_stream->setByteOrder(QDataStream::LittleEndian);
    *m_stream >> data.subchunk1_id;
    *m_stream >> data.subchunk1_size;
    *m_stream >> data.audio_format;
    *m_stream >> data.nchannels;
    *m_stream >> data.sample_rate;
    *m_stream >> data.byte_rate;
    *m_stream >> data.block_align;
    *m_stream >> data.bits_per_sample;

    m_stream->setByteOrder(QDataStream::BigEndian);
    *m_stream >> data.subchunk2_id;

    m_stream->setByteOrder(QDataStream::LittleEndian);
    *m_stream >> data.subchunk2_size;

    m_nchannels         = data.nchannels;
    m_sample_rate       = data.sample_rate;
    m_bits_per_sample   = data.bits_per_sample;
    m_nbytes            = data.subchunk2_size;
    m_nframes           = m_nbytes/(m_bits_per_sample/8);
    m_nsamples          = m_nframes/m_nchannels;

    // marker to the start of audio data
    m_stream->startTransaction();
}

void Soundfile::setBufSize(quint32 size)
{
    m_buffer = new float[size]();
}

float* Soundfile::buffer(quint32 startframe)
{
    quint32 nbytes = m_buf_size*2;
    m_stream->skipRawData ( startframe*2 );

    for ( quint32 f = 0; f < m_buf_size; ++f )
    {
        qint16 si16; *m_stream >> si16;
        m_buffer[f] = si16/65535.f;
    }

    m_stream->commitTransaction();

    return m_buffer;
}

float *Soundfile::stream()
{
    quint32 nbytes = m_buf_size*2;

    for ( quint32 f = 0; f < m_buf_size; ++f )
    {
        qint16 si16; *m_stream >> si16;
        m_buffer[f] = si16/65535.f;
    }

    return m_buffer;
}

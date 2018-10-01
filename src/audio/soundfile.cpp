#include "soundfile.hpp"
#include <QtDebug>
#include <qendian.h>

SoundfileStreamer::SoundfileStreamer(Soundfile* file) : m_soundfile(file)
{

}

SoundfileStreamer::~SoundfileStreamer()
{
    delete m_cbuffer;
}

void SoundfileStreamer::setStartSample(quint64 index)
{
    quint64 bytes_per_sample = m_soundfile->m_bits_per_sample/8;

    m_start_byte = index*m_soundfile->m_nchannels*bytes_per_sample+m_soundfile->m_metadata_size;
    m_position_byte = m_start_byte;
}

void SoundfileStreamer::setBufferSize(quint64 nsamples)
{
    m_bufsize_byte = nsamples*m_soundfile->m_nchannels*(m_soundfile->m_bits_per_sample/8);
    m_cbuffer = new char[m_bufsize_byte]();
}

void SoundfileStreamer::next(float* target)
{
    quint64 nbytes      = m_bufsize_byte;
    quint64 position    = m_position_byte;
    quint64 endframe    = position+nbytes;
    quint64 file_size   = m_soundfile->m_file_size;
    QFile* file         = m_soundfile->m_file;
    char* buf           = m_cbuffer;

    file->seek(position);

    if ( endframe > file_size && m_wrap )
    {
        quint64 f   = nbytes-(endframe-file_size);
        quint64 f2  = file_size-position;

        file->read  ( buf, f );
        file->seek  ( m_start_byte );
        buf += f;

        file->read  ( buf, f2 );
        m_position_byte = m_start_byte+f2;
        buf -= f;
    }
    else
    {
        // if endframe goes beyond end of file, qfile will fill the rest with zeroes,
        // which is what we want
        file->read(buf, nbytes);
        m_position_byte += nbytes;
    }

    for ( quint64 i = 0; i < nbytes/2; ++i )
    {
        target[i] = static_cast<float>(*buf/65535.f);
        buf += 2;
    }

    emit bufferLoaded();
}

//------------------------------------------------------------------------------------------------

Soundfile::Soundfile() : m_file(nullptr)
{

}

Soundfile::Soundfile(QString path) : m_file(new QFile(path))
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

    qDebug() << "[SOUNDFILE]" << m_path << "successfully opened";

    if ( m_path.endsWith(".wav") ) metadataWav();
}

void Soundfile::metadataWav()
{
    WavMetadata data;
    QDataStream stream(m_file);

    stream.setByteOrder(QDataStream::BigEndian);
    stream >> data.chunk_id;

    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> data.chunk_size;

    stream.setByteOrder(QDataStream::BigEndian);
    stream >> data.format;

    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> data.subchunk1_id;
    stream >> data.subchunk1_size;
    stream >> data.audio_format;
    stream >> data.nchannels;
    stream >> data.sample_rate;
    stream >> data.byte_rate;
    stream >> data.block_align;
    stream >> data.bits_per_sample;

    stream.setByteOrder(QDataStream::BigEndian);
    stream >> data.subchunk2_id;

    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> data.subchunk2_size;

    m_nchannels         = data.nchannels;
    m_sample_rate       = data.sample_rate;
    m_bits_per_sample   = data.bits_per_sample;
    m_nbytes            = data.subchunk2_size;
    m_nframes           = m_nbytes/(m_bits_per_sample/8);
    m_nsamples          = m_nframes/m_nchannels;
    m_file_size         = m_file->size();
    m_metadata_size     = WAVE_METADATA_SIZE;
}

void Soundfile::buffer(float* buffer, quint64 start_sample, quint64 len )
{
    quint64 start = start_sample*m_nchannels*(m_bits_per_sample/8)+m_metadata_size;
    quint64 length_in_frames = len*m_nchannels;

    QDataStream stream  ( m_file );
    stream.skipRawData  ( start );

    for ( quint32 f = 0; f < length_in_frames; ++f )
    {
        // convert to float
        qint16 si16; stream >> si16;
        buffer[f] = si16/65535.f;
    }
}



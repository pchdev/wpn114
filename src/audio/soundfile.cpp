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

void SoundfileStreamer::setEndSample(quint64 index)
{
    quint64 bytes_per_sample = m_soundfile->m_bits_per_sample/8;
    m_end_byte = index*m_soundfile->m_nchannels*bytes_per_sample+m_soundfile->m_metadata_size;
}

void SoundfileStreamer::setBufferSize(quint64 nsamples)
{
    m_bufsize_byte = nsamples*m_soundfile->m_nchannels*(m_soundfile->m_bits_per_sample/8);
    m_cbuffer = new char[m_bufsize_byte]();
}

void SoundfileStreamer::next(float* target)
{
    qDebug() << "[STREAMER] Preparing new buffer";

    quint16 bps         = m_soundfile->m_bits_per_sample;
    quint64 nbytes      = m_bufsize_byte;
    quint64 position    = m_position_byte;
    quint64 endframe    = position+nbytes;
    quint64 endbyte     = m_end_byte;
    QFile* file         = m_soundfile->m_file;
    char* buf           = m_cbuffer;

    file->seek(position);

    if ( endframe > endbyte && m_wrap )
    {
        quint64 f   = nbytes-(endframe-endbyte);
        quint64 f2  = endbyte-position;

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
        auto div = (2<<(bps-1))-1;
        target[i] = static_cast<float>(*buf/(float)div);
        buf += bps/8;
    }

    qDebug() << "[STREAMER] new buffer loaded";
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
    m_path = path;
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
    stream >> data.subchunk1_id;

    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> data.subchunk1_size;
    stream >> data.audio_format;
    stream >> data.nchannels;
    stream >> data.sample_rate;
    stream >> data.byte_rate;
    stream >> data.block_align;
    stream >> data.bits_per_sample;

    // there might be an extension chunk, if so, ignore it for the moment
    stream.setByteOrder(QDataStream::BigEndian);

    QByteArray chunk2(4, Qt::Uninitialized);
    stream.readRawData(chunk2.data(), 4);

    stream.setByteOrder(QDataStream::LittleEndian);
    qint32 chunk2_sz; stream >> chunk2_sz;

    if ( chunk2 != "data" )
    {
        m_metadata_size = WAVE_METADATA_SIZE+chunk2_sz+8;
        stream.skipRawData(chunk2_sz);
    }
    else m_metadata_size = WAVE_METADATA_SIZE;

    stream.setByteOrder(QDataStream::BigEndian);
    stream >> data.subchunk2_id;

    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> data.subchunk2_size;

    m_file_size         = m_file->size();
    m_nchannels         = data.nchannels;
    m_sample_rate       = data.sample_rate;
    m_bits_per_sample   = data.bits_per_sample;
    m_nbytes            = data.subchunk2_size;
    m_nframes           = m_nbytes/(m_bits_per_sample/8);
    m_nsamples          = m_nframes/m_nchannels;    

    qDebug() << "[SOUNDFILE] WAV format, metadata succesfully parsed";
    qDebug() << "[SOUNDFILE]" << m_nframes << "audio frames";
}

void Soundfile::buffer(float* buffer, quint64 start_sample, quint64 len )
{
    quint64 start = start_sample*m_nchannels*(m_bits_per_sample/8)+m_metadata_size;
    quint64 length_in_frames = len*m_nchannels;

    QDataStream stream  ( m_file );
    stream.setByteOrder ( QDataStream::LittleEndian );
    stream.skipRawData  ( start );

    for ( quint32 f = 0; f < length_in_frames; ++f )
    {
        // convert to float
        qint16 si16; stream >> si16;
        buffer[f] = si16/65535.f;
    }

    qDebug() << "[SOUNDFILE] Buffered" << length_in_frames
             << "frames ("
             << len
             << "samples)";
}



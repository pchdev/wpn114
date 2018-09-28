#ifndef SOUNDFILE_HPP
#define SOUNDFILE_HPP

#include <QObject>
#include <QFile>
#include <QDataStream>

struct WavMetadata
{
    quint32 chunk_id;
    quint32 chunk_size;
    quint32 format;

    quint32 subchunk1_id;
    quint32 subchunk1_size;
    quint16 audio_format;
    quint16 nchannels;
    quint32 sample_rate;
    quint32 byte_rate;
    quint16 block_align;
    quint16 bits_per_sample;

    quint32 subchunk2_id;
    quint32 subchunk2_size;
};

class Soundfile : public QObject
{
    Q_OBJECT

    Q_PROPERTY  ( QString path READ path WRITE setPath )
    Q_PROPERTY  ( int nchannels READ nchannels )
    Q_PROPERTY  ( int nframes READ nframes )
    Q_PROPERTY  ( int nsamples READ nsamples )
    Q_PROPERTY  ( int bufSize READ bufSize WRITE setBufSize )

    public:
    Soundfile   ( );
    Soundfile   ( QString path );

    QString path        ( ) const { return m_path; }
    quint8 nchannels    ( ) const { return m_nchannels; }
    quint64 nframes     ( ) const { return m_nframes; }
    quint64 nsamples    ( ) const { return m_nsamples; }
    quint32 bufSize     ( ) const { return m_buf_size; }

    void setBufSize     ( quint32 size );
    float* buffer         ( quint32 startframe );
    float* stream         (  );

    void setPath        ( QString path );
    void metadataWav    ( );

    signals:
    void endOfFile      ( );

    protected:
    QFile* m_file;
    QString m_path;
    quint16 m_nchannels;
    quint32 m_sample_rate;
    quint64 m_nframes;
    quint64 m_nsamples;
    quint16 m_bits_per_sample;
    quint32 m_nbytes;
    float* m_buffer;
    QDataStream* m_stream;
    quint32 m_buf_size;

};

#endif // SOUNDFILE_HPP

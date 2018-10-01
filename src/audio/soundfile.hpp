#ifndef SOUNDFILE_HPP
#define SOUNDFILE_HPP

#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QThread>

#define WAVE_METADATA_SIZE 44

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

class Soundfile;

class SoundfileStreamer : public QObject
{
    Q_OBJECT

    public:
    SoundfileStreamer   ( Soundfile* sfile = 0 );
    ~SoundfileStreamer  ( );

    void setStartSample ( quint64 index );
    void setBufferSize  ( quint64 nsamples );
    void setWrap        ( bool wrap ) { m_wrap = wrap; }

    public slots:
    void next(float* target);

    signals:
    void bufferLoaded();

    private:
    Soundfile* m_soundfile;
    bool m_wrap;
    quint64 m_start_byte;
    quint64 m_bufsize_byte;
    quint64 m_position_byte;
    char* m_cbuffer;

};

class Soundfile : public QObject
{
    Q_OBJECT

    friend class SoundfileStreamer;

    Q_PROPERTY  ( QString path READ path WRITE setPath )
    Q_PROPERTY  ( int nchannels READ nchannels )
    Q_PROPERTY  ( int nframes READ nframes )
    Q_PROPERTY  ( int nsamples READ nsamples )
    Q_PROPERTY  ( int sampleRate READ sampleRate )

    public:
    Soundfile   ( );
    Soundfile   ( QString path );

    QString path        ( ) const { return m_path; }
    quint8 nchannels    ( ) const { return m_nchannels; }
    quint64 nframes     ( ) const { return m_nframes; }
    quint64 nsamples    ( ) const { return m_nsamples; }
    quint64 sampleRate  ( ) const { return m_sample_rate; }
    void metadataWav    ( );

    void setPath      ( QString path );
    void buffer       ( float* buffer, quint64 start_sample, quint64 len );

    protected:
    QFile* m_file;
    quint64 m_file_size;
    QString m_path;
    quint16 m_nchannels;
    quint32 m_sample_rate;
    quint64 m_nframes;
    quint64 m_nsamples;
    quint32 m_nbytes;
    quint16 m_bits_per_sample;
    quint32 m_metadata_size;
};

#endif // SOUNDFILE_HPP

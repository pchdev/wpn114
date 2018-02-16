#include "fields.h"
#include <QFile>
#include <QtEndian>

#define AUDIO_BUFFER_SIZE 2 << 17

Fields::Fields() : m_path(""), m_xfade(65536), m_buffer(0), m_file(0),
    m_spos(0), m_bpos(0)
{
    SETN_IN ( 0 );
}

Fields::~Fields() {}

QAudioFormat get_audio_format(const QFile& file)
{
    QByteArray      hdr;
    QAudioFormat    fmt;
    QDataStream     str ( &file );

    //                  get the 36 first header bytes
    str.readRawData     ( hdr.data(), 36 );
    auto ptr            = hdr.data();
    ptr                 += 22;

    quint16 nchannels       = qFromLittleEndian<quint16>( ptr ); ptr += 2;
    quint32 sample_rate     = qFromLittleEndian<quint32>( ptr ); ptr += 10;
    quint16 sample_size     = qFromLittleEndian<quint16>( ptr );

    fmt.setByteOrder        ( QAudioFormat::LittleEndian );
    fmt.setSampleType       ( QAudioFormat::SignedInt );
    fmt.setCodec            ( "audio/pcm" );
    fmt.setChannelCount     ( nchannels );
    fmt.setSampleRate       ( sample_rate );
    fmt.setSampleSize       ( sample_size );

    return fmt;
}

void Fields::setNumInputs(const quint16) {}
void Fields::classBegin() {}
void Fields::componentComplete()
{
     m_file = new QFile ( m_path );

    if ( !m_file->open( QIODevice::ReadOnly ))
    {
         qDebug() << "couln't open file";
         return;
    }

    auto format         = get_audio_format( *m_file );
    quint64 nbytes      = m_file->size() - 44;
    quint16 bpf         = format.sampleSize()/8; // bytes per frame
    quint16 nframes     = nbytes/bpf;
    quint64 nsamples    = nframes/format.channelCount();

    m_buffer = new QAudioBuffer( AUDIO_BUFFER_SIZE, format );

    SETN_OUT ( format.channelCount() );
    INITIALIZE_AUDIO_IO;

    //                      prepare first audio buffer
    m_stream                = new QDataStream( *m_file );
    m_stream->setByteOrder  ( QDataStream::LittleEndian );
    auto ptr                = m_buffer->data<qint8>();

    m_stream->skipRawData     ( 44 );
    m_stream->readRawData     ( ptr, AUDIO_BUFFER_SIZE * bpf );

    emit bufferReady();
}

float** Fields::process(const quint16 nsamples)
{
    quint32 spos    = m_spos;
    quint32 bpos    = m_bpos;

    return m_outputs;
}

uint32_t Fields::xfade() const
{
    return m_xfade;
}

QString Fields::path() const
{
    return m_path;
}

void Fields::setXfade(const uint32_t xfadelen)
{
    m_xfade = xfadelen;
}

void Fields::setPath(const QString path)
{
    m_path = path;
}

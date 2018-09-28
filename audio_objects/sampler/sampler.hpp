#ifndef SAMPLER_HPP
#define SAMPLER_HPP

#include <src/audio/audio.hpp>
#include <src/audio/soundfile.hpp>
#include <QQmlParserStatus>

class Sampler : public StreamNode, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY  ( QString path READ path WRITE setPath )
    Q_PROPERTY  ( bool loop READ loop WRITE setLoop )
    Q_PROPERTY  ( int xfade READ xfade WRITE setXfade )
    Q_PROPERTY  ( int attack READ attack WRITE setAttack )
    Q_PROPERTY  ( int release READ release WRITE setRelease )
    Q_PROPERTY  ( qreal start READ start WRITE setStart )
    Q_PROPERTY  ( qreal length READ length WRITE setLength )
    Q_PROPERTY  ( qreal rate READ rate WRITE setRate )
    Q_PROPERTY  ( bool stream READ stream WRITE setStream )

    public:
    Sampler();

    virtual void componentComplete() override;
    virtual void classBegin() override {}

    virtual float** userProcess(float**, qint64 le) override;
    virtual void userInitialize(qint64) override;

    QString path        ( ) const { return m_path; }
    bool loop           ( ) const { return m_loop; }
    quint32 xfade       ( ) const { return m_xfade; }
    quint32 attack      ( ) const { return m_attack; }
    quint32 release     ( ) const { return m_release; }
    qreal start         ( ) const { return m_start; }
    qreal length        ( ) const { return m_length; }
    qreal rate          ( ) const { return m_rate; }
    bool stream         ( ) const { return m_stream; }

    void setPath        ( QString path );
    void setLoop        ( bool loop );
    void setXfade       ( quint32 xfade );
    void setAttack      ( quint32 attack );
    void setRelease     ( quint32 release );
    void setStart       ( qreal start );
    void setLength      ( qreal length );
    void setRate        ( qreal rate );
    void setStream      ( bool stream );

    signals:
    void readyRead ();

    private:
    Soundfile* m_sfile;
    float* m_buffer;

    QString m_path;
    bool m_loop;
    bool m_stream;
    quint32 m_xfade;
    quint32 m_attack;
    quint32 m_release;
    qreal m_start;
    qreal m_length;
    qreal m_rate;
};

#endif // SAMPLER_HPP

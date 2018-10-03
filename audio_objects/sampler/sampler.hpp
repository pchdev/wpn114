#ifndef SAMPLER_HPP
#define SAMPLER_HPP

#include <src/audio/audio.hpp>
#include <src/audio/soundfile.hpp>
#include <QQmlParserStatus>
#include <QThread>

#define ENV_RESOLUTION 32768
#define BUFSTREAM_MAX_XFADELEN 5
// in seconds
#define BUFSTREAM_NSAMPLES_DEFAULT 131072
// approx. 3 seconds buffer

class StreamSampler : public StreamNode, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY  ( QString path READ path WRITE setPath )
    Q_PROPERTY  ( bool loop READ loop WRITE setLoop )
    Q_PROPERTY  ( int xfade READ xfade WRITE setXfade )
    Q_PROPERTY  ( int attack READ attack WRITE setAttack )
    Q_PROPERTY  ( int release READ release WRITE setRelease )
    Q_PROPERTY  ( qreal start READ start WRITE setStart )
    Q_PROPERTY  ( qreal end READ end WRITE setEnd )
    Q_PROPERTY  ( qreal length READ length WRITE setLength )

    public:
    StreamSampler();
    ~StreamSampler() override;

    virtual void componentComplete() override;
    virtual void classBegin() override {}

    virtual float** userProcess(float**, qint64 le) override;
    virtual void userInitialize(qint64) override;

    Q_INVOKABLE void start  ( );
    Q_INVOKABLE void stop   ( );

    QString path        ( ) const { return m_path; }
    bool loop           ( ) const { return m_loop; }
    quint32 xfade       ( ) const { return m_xfade; }
    quint32 attack      ( ) const { return m_attack; }
    quint32 release     ( ) const { return m_release; }
    qreal start         ( ) const { return m_start; }
    qreal end           ( ) const { return m_end; }
    qreal length        ( ) const { return m_length; }
    qreal rate          ( ) const { return m_rate; }

    void setPath        ( QString path );
    void setLoop        ( bool loop );
    void setXfade       ( quint32 xfade );
    void setAttack      ( quint32 attack );
    void setRelease     ( quint32 release );
    void setStart       ( qreal start );
    void setEnd         ( qreal end );
    void setLength      ( qreal length );
    void setRate        ( qreal rate );

    public slots:
    void onNextBufferReady();

    signals:
    void next (float*);
    void fileLengthChanged ();

    private:
    Soundfile* m_soundfile;
    SoundfileStreamer* m_streamer;
    QThread m_streamer_thread;

    bool m_next_buffer_ready;
    bool m_first_play;
    bool m_releasing;
    bool m_playing;

    quint64 m_buffer_size;
    quint64 m_play_size;
    float* m_xfade_buffer;
    float* m_current_buffer;
    float* m_next_buffer;

    quint64 m_phase;
    quint64 m_stream_phase;
    quint64 m_xfade_buf_phase;
    float m_attack_phase;
    float m_release_phase;
    float m_xfade_phase;
    float m_attack_inc;
    float m_release_inc;
    float m_xfade_inc;

    quint64 m_attack_end;
    quint64 m_xfade_point;
    quint64 m_xfade_length;

    float m_attack_env  [ ENV_RESOLUTION ];
    float m_release_env [ ENV_RESOLUTION ];

    // properties
    QString m_path;
    bool m_loop;
    quint32 m_xfade;
    quint32 m_attack;
    quint32 m_release;
    qreal m_start;
    qreal m_end;
    qreal m_length;
    qreal m_rate;
};

class Sampler : public StreamNode, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY  ( QString path READ path WRITE setPath )
    Q_PROPERTY  ( bool loop READ loop WRITE setLoop )
    Q_PROPERTY  ( int xfade READ xfade WRITE setXfade )
    Q_PROPERTY  ( int attack READ attack WRITE setAttack )
    Q_PROPERTY  ( int release READ release WRITE setRelease )
    Q_PROPERTY  ( qreal start READ start WRITE setStart )
    Q_PROPERTY  ( qreal end READ end WRITE setEnd )
    Q_PROPERTY  ( qreal length READ length WRITE setLength )
    Q_PROPERTY  ( qreal rate READ rate WRITE setRate )

    public:
    Sampler();
    ~Sampler() override;

    virtual void componentComplete() override;
    virtual void classBegin() override {}

    virtual float** userProcess(float**, qint64 le) override;
    virtual void userInitialize(qint64) override;

    Q_INVOKABLE void start  ( );
    Q_INVOKABLE void stop   ( );

    QString path        ( ) const { return m_path; }
    bool loop           ( ) const { return m_loop; }
    quint32 xfade       ( ) const { return m_xfade; }
    quint32 attack      ( ) const { return m_attack; }
    quint32 release     ( ) const { return m_release; }
    qreal start         ( ) const { return m_start; }
    qreal end           ( ) const { return m_end; }
    qreal length        ( ) const { return m_length; }
    qreal rate          ( ) const { return m_rate; }

    void setPath        ( QString path );
    void setLoop        ( bool loop );
    void setXfade       ( quint32 xfade );
    void setAttack      ( quint32 attack );
    void setRelease     ( quint32 release );
    void setStart       ( qreal start );
    void setEnd         ( qreal end );
    void setLength      ( qreal length );
    void setRate        ( qreal rate );

    private:    
    Soundfile* m_soundfile;
    float* m_buffer;
    quint64 m_buffer_size;

    bool m_first_play;
    bool m_releasing;
    bool m_playing;

    quint64 m_phase;
    float m_attack_phase;
    float m_release_phase;
    float m_xfade_phase;
    quint64 m_attack_end;
    quint64 m_release_end;
    quint64 m_xfade_point;
    quint64 m_xfade_length;

    float m_attack_inc;
    float m_release_inc;
    float m_xfade_inc;

    float m_attack_env  [ ENV_RESOLUTION ];
    float m_release_env [ ENV_RESOLUTION ];

    // properties
    QString m_path;
    bool m_loop;
    quint32 m_xfade;
    quint32 m_attack;
    quint32 m_release;
    qreal m_start;
    qreal m_end;
    qreal m_length;
    qreal m_rate;
};

#endif // SAMPLER_HPP

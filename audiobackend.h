#ifndef AUDIOBACKEND_H
#define AUDIOBACKEND_H

#include <QQuickItem>
#include <QAudioOutput>
#include "audiomaster.h"

class AudioObject;

class AudioSend : public QObject, QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY  ( AudioEffectObject* target READ target WRITE setTarget NOTIFY targetChanged )
    Q_PROPERTY  ( float level READ level WRITE setLevel )
    Q_PROPERTY  ( bool prefader READ prefader WRITE setPrefader )
    Q_PROPERTY  ( int offset READ offset WRITE setOffset )
    Q_PROPERTY  ( bool active READ active WRITE setActive )
    Q_PROPERTY  ( bool muted READ muted WRITE setMuted )

public:
    AudioSend();
    ~AudioSend();

    void componentComplete() override;
    void classBegin() override;

    AudioEffectObject* target() const;
    float level() const;
    bool prefader() const;
    int offset() const;
    bool active() const;
    bool muted() const;

    void setTarget(const AudioEffectObject*);
    void setLevel(const float);
    void setPrefader(const bool);
    void setOffset(const int);
    void setActive(const bool);
    void setMuted(const bool);

private:

    float                   m_level;
    bool                    m_prefader;
    int                     m_offset;
    bool                    m_active;
    bool                    m_muted;
    AudioEffectObject*      m_target;

};

class AudioObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY  ( int offset READ offset WRITE setOffset )
    Q_PROPERTY  ( float level READ level WRITE setLevel )
    Q_PROPERTY  ( int numOutputs READ numOutputs WRITE setNumOutputs NOTIFY numOutputsChanged )
    Q_PROPERTY  ( bool active READ active WRITE setActive NOTIFY activeChanged )
    Q_PROPERTY  ( bool muted READ active WRITE setMuted NOTIFY mutedChanged )
    Q_PROPERTY  ( QQmlListProperty<AudioSend> sends READ sends )
    Q_CLASSINFO ( "DefaultProperty", "sends")

public:
    ~AudioObject();

    virtual float** process(const quint16 nsamples) = 0;
    QQmlListProperty<AudioSend> sends();

    float       level           () const;
    quint16     offset          () const;
    quint16     numOutputs      () const;
    bool        active          () const;
    bool        muted           () const;

    void setLevel               ( const float );
    void setOffset              ( const quint16 );
    void setNumOutputs          ( const quint16 );
    void setActive              ( const bool );
    void setMuted               ( const bool );

signals:
    void numOutputsChanged      ();
    void activeChanged          ();
    void mutedChanged           ();

protected:
    bool                    m_active;
    bool                    m_muted;
    float                   m_level;
    float**                 m_outputs;
    quint16                 m_offset;
    quint16                 m_num_outputs;
    QList<AudioSend*>       m_sends;

#define SAMPLERATE      AudioBackend::sampleRate()
#define SR              AudioBackend::sampleRate()
#define BLOCKSIZE       AudioBackend::blockSize()
#define SETN_OUT(n)     m_num_outputs = n;
#define SET_OFFSET(o)   m_offset = o;
#define OUT             m_outputs

#define ZEROBUF( target, sz ) \
    for(int i = 0; i < sz; ++i ) \
    std::memset(target[i], 0.f, sizeof(float)*BLOCKSIZE);

#define IOALLOC(target,n) \
    target = new float*[n]; \
    for(int i = 0; i < n; ++i) \
    target[i] = (float*) std::calloc(n*BLOCKSIZE, sizeof(float));

#define IODEALLOC(target, n)                                \
    for(int i = 0; i < n; ++i)                              \
    delete target[i];                                       \
    delete target;

#define INITIALIZE_AUDIO_OUTPUTS                            \
    IOALLOC ( m_outputs, m_num_outputs );                   \

inline void inbufmerge(
    float** b1, float** b2, uint16_t nin,
    uint16_t unout, uint16_t uoff, uint16_t nsamples )
    {
        for(int ch = 0; ch < nin; ++ch)
            if ( ch >= uoff && ch < uoff+unout )
                for(int s = 0; s < nsamples; ++s)
                    b1[ch][s] += b2[ch-uoff][s];
    }
};

class AudioEffectObject : public AudioObject
{
    Q_OBJECT
    Q_PROPERTY  ( int numInputs READ numInputs WRITE setNumInputs NOTIFY numInputsChanged )
    Q_PROPERTY  ( QQmlListProperty<AudioObject> inputs READ inputs )
    Q_PROPERTY  ( QQmlListProperty<AudioSend> receives READ receives )
    Q_CLASSINFO ( "DefaultProperty", "inputs")

public:
    ~AudioEffectObject();
    quint16 numInputs() const;
    void setNumInputs(const quint16);

    QQmlListProperty<AudioObject>   inputs();
    QQmlListProperty<AudioSend>     receives();

    void addReceive(const AudioSend& receive);

signals:
    void numInputsChanged();

protected:
#define IN              m_inbuf
#define SETN_IN(n)      m_num_inputs = n;

#define INITIALIZE_AUDIO_IO \
        INITIALIZE_AUDIO_OUTPUTS \
        IOALLOC ( m_inbuf, m_num_inputs );

#define GET_INPUTS                                                  \
    auto inputs     = m_inputs;                                     \
    auto nin        = m_num_inputs;                                 \
    float** in      = IN;                                           \
    for( const auto& input : inputs ) {                             \
        uint16_t unout  = input->numOutputs();                      \
        uint16_t uoff   = input->offset();                          \
        float** buf     = input->process(nsamples);                 \
        inbufmerge(in, buf, nin, unout, uoff, nsamples); }

#define GET_RECEIVES                                                  \
    auto sends      = m_sends;

protected:
    float**                 m_inbuf;
    quint16                 m_num_inputs;
    QList<AudioObject*>     m_inputs;
    QList<AudioSend*>       m_receives;

};

// -------------------------------------------------------------------------------------

class AudioMaster;

class AudioBackend : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES ( QQmlParserStatus )
    Q_CLASSINFO  ( "DefaultProperty", "masters")

    Q_PROPERTY( QString device READ device WRITE setDevice )
    Q_PROPERTY( int numInputs READ numInputs WRITE setNumInputs )
    Q_PROPERTY( int numOutputs READ numOutputs WRITE setNumOutputs )
    Q_PROPERTY( int sampleRate READ sampleRate WRITE setSampleRate )
    Q_PROPERTY( int blockSize READ blockSize WRITE setBlockSize)
    Q_PROPERTY( bool active READ active WRITE setActive NOTIFY activeChanged )
    Q_PROPERTY( bool muted READ muted WRITE setMuted NOTIFY mutedChanged )
    Q_PROPERTY( QQmlListProperty<AudioMaster> masters READ masters )

public:
    AudioBackend();

    virtual void classBegin();
    virtual void componentComplete();
    QQmlListProperty<AudioMaster> masters();

    Q_INVOKABLE void configure  ();
    Q_INVOKABLE void start      ();
    Q_INVOKABLE void stop       ();
    Q_INVOKABLE void suspend    ();
    Q_INVOKABLE void resume     ();

    static quint32 sampleRate   ();
    static quint16 blockSize    ();
    static quint16 numInputs    ();
    static quint16 numOutputs   ();

    static void setSampleRate   (quint32);
    static void setBlockSize    (const quint16);

    static quint32  m_sample_rate;
    static quint16  m_block_size;
    static quint16  m_num_inputs;
    static quint16  m_num_outputs;

protected slots:
    void onAudioStateChanged    (QAudio::State);

private:
    QString device              () const;
    bool active                 () const;
    bool muted                  () const;

    void setDevice              (const QString&);
    void setNumInputs           (const quint16);
    void setNumOutputs          (const quint16);
    void setActive              (const bool);
    void setMuted               (const bool);

signals:
    void activeChanged          ();
    void mutedChanged           ();

private:
    bool                    m_muted;
    bool                    m_active;
    QString                 m_device;
    QAudioFormat            m_format;
    QAudioOutput*           m_output;
    QList<AudioMaster*>     m_masters;
};

#endif // AUDIOBACKEND_H

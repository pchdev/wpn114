#pragma once

#include <QObject>
#include <QQmlListProperty>

#include <QQmlParserStatus>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>
#include <QVector>

struct StreamProperties
{
    quint32 sample_rate;
    quint16 block_size;
};

class StreamNode : public QObject
{
    Q_OBJECT

    Q_PROPERTY  ( bool mute READ mute WRITE setMute NOTIFY muteChanged )
    Q_PROPERTY  ( bool active READ active WRITE setActive NOTIFY activeChanged )
    Q_PROPERTY  ( int numInputs READ numInputs WRITE setNumInputs NOTIFY numInputsChanged )
    Q_PROPERTY  ( int numOutputs READ numOutputs WRITE setNumOutputs NOTIFY numOutputsChanged )
    Q_PROPERTY  ( qreal level READ level WRITE setLevel NOTIFY levelChanged )

    public:
    virtual float** process ( float** buf, qint64 le ) { return nullptr; }
    virtual float** userProcess ( float** buf, qint64 le ) = 0;
    virtual void initialize(StreamProperties properties) { Q_UNUSED(properties); }
    virtual void userInitialize(qint64) = 0;

    static void allocateBuffer(float**& buffer, quint16 nchannels, quint16 nsamples );
    static void resetBuffer(float**& buffer, quint16 nchannels, quint16 nsamples );

    uint16_t numInputs   ( ) const { return m_num_inputs; }
    uint16_t numOutputs  ( ) const { return m_num_outputs; }
    qreal level          ( ) const { return m_level; }
    bool mute            ( ) const { return m_mute; }
    bool active          ( ) const { return m_active; }

    virtual void setNumInputs    ( uint16_t num_inputs );
    virtual void setNumOutputs   ( uint16_t num_outputs );
    virtual void setMute         ( bool mute );
    virtual void setActive       ( bool active );

    void setLevel ( qreal level );

    signals:
    void muteChanged        ( );
    void activeChanged      ( );
    void numInputsChanged   ( );
    void numOutputsChanged  ( );
    void levelChanged       ( );

    protected:
    StreamProperties m_stream_properties;
    qreal m_level;
    uint16_t m_num_inputs;
    uint16_t m_num_outputs;
    bool m_mute;
    bool m_active;

    #define SAMPLERATE m_stream_properties.sample_rate
};

class OutStreamNode;

class InStreamNode : public StreamNode
{
    Q_OBJECT
    Q_CLASSINFO ( "DefaultProperty", "inputs" )

    Q_PROPERTY  ( QQmlListProperty<OutStreamNode> inputs READ inputs )

    public:
    virtual float** process ( float** buf, qint64 le ) override;
    virtual void initialize(StreamProperties properties) override;

    QQmlListProperty<OutStreamNode>  inputs();
    const QList<OutStreamNode*>&     getInputs() const;

    protected:
    QList<OutStreamNode*> m_inputs;
    float** m_in;
};

class OutStreamNode: public StreamNode
{
    Q_OBJECT
    Q_CLASSINFO ( "DefaultProperty", "outputs" )

    Q_PROPERTY  ( QVector<int> parentChannels READ parentChannels WRITE setParentChannels )
    Q_PROPERTY  ( QQmlListProperty<InStreamNode> outputs READ outputs)

    public:
    virtual float** process ( float** buf, qint64 le ) override;
    virtual void initialize(StreamProperties properties) override;

    QVector<int> parentChannels() const { return m_pch; }
    void setParentChannels(QVector<int> pch);

    QQmlListProperty<InStreamNode>  outputs();
    const QList<InStreamNode*>&     getOutputs() const;

    protected:
    float** m_out;
    QList<InStreamNode*> m_outputs;
    QVector<int> m_pch;
};

// virtual inheritance not possible with QObject, so we have to reset it all
class IOStreamNode : public StreamNode
{
    Q_OBJECT
    Q_CLASSINFO ( "DefaultProperty", "outputs" )

    Q_PROPERTY  ( QQmlListProperty<OutStreamNode> inputs READ inputs )
    Q_PROPERTY  ( QVector<int> parentChannels READ parentChannels WRITE setParentChannels )
    Q_PROPERTY  ( QQmlListProperty<InStreamNode> outputs READ outputs)

    public:
    virtual float** process ( float** buf, qint64 le ) override;
    virtual void initialize(StreamProperties properties) override;

    QVector<int> parentChannels() const { return m_pch; }
    void setParentChannels(QVector<int> pch);

    QQmlListProperty<OutStreamNode>  inputs();
    const QList<OutStreamNode*>&     getInputs() const;

    QQmlListProperty<InStreamNode>  outputs();
    const QList<InStreamNode*>&     getOutputs() const;

    protected:
    QList<OutStreamNode*> m_inputs;
    float** m_in;
    float** m_out;
    QList<InStreamNode*> m_outputs;
    QVector<int> m_pch;
};

class WorldStream : public QIODevice, public QQmlParserStatus
{
    Q_OBJECT
    Q_CLASSINFO     ( "DefaultProperty", "inputs" )
    Q_INTERFACES    ( QQmlParserStatus )

    Q_PROPERTY      ( bool mute READ mute WRITE setMute NOTIFY muteChanged )
    Q_PROPERTY      ( bool active READ active WRITE setActive NOTIFY activeChanged )
    Q_PROPERTY      ( int numInputs READ numInputs WRITE setNumInputs NOTIFY numInputsChanged )
    Q_PROPERTY      ( int numOutputs READ numOutputs WRITE setNumOutputs NOTIFY numOutputsChanged )
    Q_PROPERTY      ( qreal level READ level WRITE setLevel NOTIFY levelChanged )
    Q_PROPERTY      ( int sampleRate READ sampleRate WRITE setSampleRate NOTIFY sampleRateChanged )
    Q_PROPERTY      ( int blockSize READ blockSize WRITE setBlockSize NOTIFY blockSizeChanged )
    Q_PROPERTY      ( QString inDevice READ inDevice WRITE setInDevice NOTIFY inDeviceChanged )
    Q_PROPERTY      ( QString outDevice READ outDevice WRITE setOutDevice NOTIFY outDeviceChanged )
    Q_PROPERTY      ( QQmlListProperty<OutStreamNode> inputs READ inputs )

    public:    
    WorldStream();

    virtual void classBegin()                           override {}
    virtual void componentComplete()                    override;
    virtual qint64 readData ( char*, qint64 )           override;
    virtual qint64 writeData ( const char*, qint64 )    override;
    virtual qint64 bytesAvailable ( )                   const override;

    Q_INVOKABLE void start  ();
    Q_INVOKABLE void stop   ();

    QQmlListProperty<OutStreamNode>  inputs();
    const QList<OutStreamNode*>&     getInputs() const;

    uint16_t numInputs      ( ) const { return m_num_inputs; }
    uint16_t numOutputs     ( ) const { return m_num_outputs; }
    qreal level             ( ) const { return m_level; }
    bool mute               ( ) const { return m_mute; }
    bool active             ( ) const { return m_active; }
    uint32_t sampleRate     ( ) const { return m_sample_rate; }
    uint16_t blockSize      ( ) const { return m_block_size; }
    QString inDevice        ( ) const { return m_in_device; }
    QString outDevice       ( ) const { return m_out_device; }

    void setSampleRate   ( uint32_t sample_rate );
    void setBlockSize    ( uint16_t block_size );
    void setInDevice     ( QString device );
    void setOutDevice    ( QString device );
    void setNumInputs    ( uint16_t num_inputs );
    void setNumOutputs   ( uint16_t num_outputs );
    void setMute         ( bool mute );
    void setActive       ( bool active );
    void setLevel        ( qreal level );

    protected slots:
    void onAudioStateChanged ( QAudio::State ) const;

    signals:
    void sampleRateChanged  ( );
    void blockSizeChanged   ( );
    void inDeviceChanged    ( );
    void outDeviceChanged   ( );
    void muteChanged        ( );
    void activeChanged      ( );
    void numInputsChanged   ( );
    void numOutputsChanged  ( );
    void levelChanged       ( );

    private:
    qreal m_level;
    uint16_t m_num_inputs;
    uint16_t m_num_outputs;
    bool m_mute;
    bool m_active;
    uint32_t m_sample_rate;
    uint16_t m_block_size;
    QString m_in_device;
    QString m_out_device;
    QAudioFormat m_format;
    QAudioInput* m_input;
    QAudioOutput* m_output;
    float** m_pool;
    float* m_buf;

    QList<OutStreamNode*> m_inputs;
};

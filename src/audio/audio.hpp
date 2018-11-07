#pragma once

#include <QObject>
#include <QQmlListProperty>

#include <QQmlParserStatus>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>
#include <QVector>
#include <src/oscquery/device.hpp>
#include <QThread>

struct StreamProperties
{
    quint32 sample_rate;
    quint16 block_size;
};

/*class ControlNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY  ( bool audioRate READ audioRate WRITE setAudioRate )
    Q_PROPERTY  ( qreal from READ from WRITE setFrom )

};*/

class WorldStream;

class StreamNode : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_CLASSINFO     ( "DefaultProperty", "subnodes" )
    Q_INTERFACES    ( QQmlParserStatus )

    Q_PROPERTY  ( bool mute READ mute WRITE setMute NOTIFY muteChanged )
    Q_PROPERTY  ( bool active READ active WRITE setActive NOTIFY activeChanged )
    Q_PROPERTY  ( int numInputs READ numInputs WRITE setNumInputs NOTIFY numInputsChanged )
    Q_PROPERTY  ( int numOutputs READ numOutputs WRITE setNumOutputs NOTIFY numOutputsChanged )
    Q_PROPERTY  ( QVariant parentChannels READ parentChannels WRITE setParentChannels )
    Q_PROPERTY  ( qreal level READ level WRITE setLevel NOTIFY levelChanged )
    Q_PROPERTY  ( qreal dBlevel READ dBlevel WRITE setDBlevel NOTIFY dBlevelChanged )
    Q_PROPERTY  ( QQmlListProperty<StreamNode> subnodes READ subnodes )
    Q_PROPERTY  ( StreamNode* parentStream READ parentStream WRITE setParentStream )

    Q_PROPERTY  ( QString exposePath READ exposePath WRITE setExposePath )
    Q_PROPERTY  ( WPNDevice* exposeDevice READ exposeDevice WRITE setExposeDevice )

    public:
    StreamNode();
    ~StreamNode() override;

    static void deleteBuffer    ( float**& buffer, quint16 nchannels, quint16 nsamples );
    static void allocateBuffer  ( float**& buffer, quint16 nchannels, quint64 nsamples );
    static void resetBuffer     ( float**& buffer, quint16 nchannels, quint16 nsamples );
    static void applyGain       ( float**& buffer, quint16 nchannels, quint16 nsamples, float gain );
    static void mergeBuffers    ( float**& lhs, float **rhs, quint16 lnchannels,
                                  quint16 rnchannels, quint16 nsamples );

    virtual void preinitialize  ( StreamProperties properties);
    virtual void initialize     ( qint64 ) = 0;
    virtual float** preprocess  ( float** buf, qint64 le );
    virtual float** process     ( float** buf, qint64 le ) = 0;

    virtual void componentComplete  ( ) override;
    virtual void classBegin         ( ) override;

    virtual void expose(WPNNode*)   { }

    QQmlListProperty<StreamNode>  subnodes();
    const QVector<StreamNode*>&   getSubnodes() const { return m_subnodes; }

    void appendSubnode      ( StreamNode* );
    int subnodesCount       ( ) const;
    StreamNode* subnode     ( int ) const;
    void clearSubnodes      ( );

    float** inputBuffer  ( ) const { return m_in; }
    float** outputBuffer ( ) const { return m_out; }
    uint16_t numInputs   ( ) const { return m_num_inputs; }
    uint16_t numOutputs  ( ) const { return m_num_outputs; }
    uint16_t maxOutputs  ( ) const { return m_max_outputs; }
    qreal level          ( ) const { return m_level; }
    bool mute            ( ) const { return m_mute; }
    bool active          ( ) const { return m_active; }
    qreal dBlevel        ( ) const { return m_db_level; }    
    bool qml             ( ) const { return m_qml; }

    QString exposePath          ( ) const { return m_exp_path; }
    WPNDevice* exposeDevice     ( ) const { return m_exp_device; }
    StreamNode* parentStream    ( ) const { return m_parent_stream; }

    virtual void setMute    ( bool mute );
    virtual void setActive  ( bool active );
    void setNumInputs       ( uint16_t num_inputs );
    void setNumOutputs      ( uint16_t num_outputs );
    void setMaxOutputs      ( uint16_t max_outputs );
    void setExposePath      ( QString path );
    void setExposeDevice    ( WPNDevice* device );
    void setParentStream    ( StreamNode* stream );

    QVector<quint16> parentChannelsVec ( ) const;
    QVariant parentChannels ( ) const { return m_parent_channels; }
    void setParentChannels  ( QVariant pch );

    void setLevel       ( qreal level );
    void setLevelNoDb   ( qreal level );
    void setDBlevel     ( qreal db );

    signals:
    void muteChanged        ( );
    void activeChanged      ( );
    void numInputsChanged   ( );
    void numOutputsChanged  ( );
    void levelChanged       ( );
    void exposePathChanged  ( );
    void dBlevelChanged     ( );

    protected:
    static void appendSubnode     ( QQmlListProperty<StreamNode>*, StreamNode* );
    static int subnodesCount      ( QQmlListProperty<StreamNode>* );
    static StreamNode* subnode    ( QQmlListProperty<StreamNode>*, int );
    static void clearSubnodes     ( QQmlListProperty<StreamNode>* );

    StreamProperties m_stream_properties;
    qreal m_level;
    qreal m_db_level;
    uint16_t m_num_inputs;
    uint16_t m_num_outputs;
    uint16_t m_max_outputs;
    bool m_mute;
    bool m_active;
    bool m_qml = false;

    float** m_in;
    float** m_out;

    QVariant m_parent_channels;
    QVector<StreamNode*> m_subnodes;

    QString m_exp_path;
    WPNDevice* m_exp_device;
    WPNNode* m_exp_node;
    StreamNode* m_parent_stream;

    #define SAMPLERATE m_stream_properties.sample_rate
    #define SETN_OUT(n) setNumOutputs(n);
    #define SETN_IN(n) setNumInputs(n);
};

class WorldStream;

class AudioStream : public QIODevice
{
     Q_OBJECT

    public:
    AudioStream  ( const WorldStream& world, QAudioFormat format, QAudioDeviceInfo info);
    ~AudioStream ( ) override;

    virtual qint64 readData  ( char*, qint64 )        override;
    virtual qint64 writeData ( const char*, qint64 )  override;
    virtual qint64 bytesAvailable ( )                 const override;

    qint64 uclock( ) const;

    signals:
    void tick       ( qint64 );

    public slots:
    void onBufferProcessed ( );
    void configure  ( );
    void start      ( );
    void stop       ( );
    void exit       ( );
    void restart    ( );

    protected slots:
    void onAudioStateChanged ( QAudio::State );

    private:
    bool m_active = false;
    qint64 m_clock;
    WorldStream const& m_world;
    QAudioFormat m_format;
    QAudioDeviceInfo m_device_info;
    QAudioInput* m_input;
    QAudioOutput* m_output;
    float** m_pool;

};

class WorldStream : public StreamNode
{
    Q_OBJECT

    Q_PROPERTY  ( int sampleRate READ sampleRate WRITE setSampleRate NOTIFY sampleRateChanged )
    Q_PROPERTY  ( int blockSize READ blockSize WRITE setBlockSize NOTIFY blockSizeChanged )
    Q_PROPERTY  ( QString inDevice READ inDevice WRITE setInDevice NOTIFY inDeviceChanged )
    Q_PROPERTY  ( QString outDevice READ outDevice WRITE setOutDevice NOTIFY outDeviceChanged )
    Q_PROPERTY  ( QQmlListProperty<StreamNode> inserts READ inserts )

    friend class AudioStream;

    public:    
    WorldStream();
    ~WorldStream();

    virtual void initialize     ( qint64 ) override {}
    virtual float** process     ( float**, qint64 ) override {}

    virtual void componentComplete() override;

    Q_INVOKABLE void start  ( );
    Q_INVOKABLE void stop   ( );

    uint32_t sampleRate     ( ) const { return m_sample_rate; }
    uint16_t blockSize      ( ) const { return m_block_size; }
    QString inDevice        ( ) const { return m_in_device; }
    QString outDevice       ( ) const { return m_out_device; }

    void setSampleRate   ( uint32_t sample_rate );
    void setBlockSize    ( uint16_t block_size );
    void setInDevice     ( QString device );
    void setOutDevice    ( QString device );

    AudioStream* stream () { return m_stream; }

    QQmlListProperty<StreamNode>  inserts();
    const QVector<StreamNode*>&   getInserts() const { return m_inserts; }

    void appendInsert      ( StreamNode* );
    int insertsCount       ( ) const;
    StreamNode* insert     ( int ) const;
    void clearInserts      ( );

    signals:    
    void tick               ( qint64 tick );
    void configure          ( );
    void startStream        ( );
    void stopStream         ( );
    void exit               ( );
    void sampleRateChanged  ( );
    void blockSizeChanged   ( );
    void inDeviceChanged    ( );
    void outDeviceChanged   ( );

    protected:
    static void appendInsert     ( QQmlListProperty<StreamNode>*, StreamNode* );
    static int insertsCount      ( QQmlListProperty<StreamNode>* );
    static StreamNode* insert    ( QQmlListProperty<StreamNode>*, int );
    static void clearInserts     ( QQmlListProperty<StreamNode>* );

    private:
    uint32_t m_sample_rate;
    uint16_t m_block_size;
    QString m_in_device;
    QString m_out_device;
    AudioStream* m_stream;
    QThread m_stream_thread;
    qint64 m_clock;

    QVector<StreamNode*> m_inserts;
};

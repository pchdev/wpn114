#ifndef BACKEND2_HPP
#define BACKEND2_HPP

#include <cstdint>
#include <vector>
#include "audiobackend.hpp"
#include "audiomaster.hpp"

using namespace std;

class StreamNode : public QObject //--------------------------------- AUDIO_ELEMENT
{
    Q_OBJECT

    Q_PROPERTY  ( bool active READ active WRITE set_active )
    Q_PROPERTY  ( bool muted READ muted WRITE set_muted )

    public: // -----------------------------------------------------
    virtual void process ( float**&, const uint16_t nsamples ) = 0;

    virtual bool active  ( ) const  { return m_active; }
    virtual bool muted   ( ) const  { return m_muted; }

    virtual void set_active ( bool a) { m_active = a; }
    virtual void set_muted  ( bool m) { m_muted = m;  }

    const Stream& stream() const { return *m_stream; }
    const StreamSegment& segment() const { return *m_segment; }

    void set_stream(const Stream& stream) { m_stream = &stream; }
    void set_segment(const StreamSegment& segment) { m_segment = &segment; }

    protected: //---------------------------------------------------
    bool  m_active;
    bool  m_muted;

    Stream*         m_stream;
    StreamSegment*  m_segment;

#define ON_COMPONENT_COMPLETED(obj)                         \
    void obj::classBegin() {}                               \
    void obj::componentComplete()

};

class InStreamNode : public StreamNode //-------------------------------- IN_ELEMENT
{
    Q_OBJECT
    Q_CLASSINFO     ( "DefaultProperty", "inputs" )

    Q_PROPERTY  ( int numInputs READ n_inputs WRITE setn_inputs )
    Q_PROPERTY  ( QQmlListProperty<OutStreamNode> inputs READ inputs )

    public: // ------------------------------------------------------
    QQmlListProperty<OutStreamNode> inputs();
    const QList<OutStreamNode*>& get_inputs() const;

    virtual void add_receive ( const Fork& send );
    void has_receives () const { return !m_receives.empty(); }
    const QList<Fork*>& receives() const { return m_receives; }

    virtual uint16_t n_inputs() const;
    virtual void setn_inputs(uint16_t);

    private: //------------------------------------------------------
    QList<OutStreamNode*> m_inputs;
    QList<Fork*> m_receives;
    uint16_t m_n_inputs;

};

class OutStreamNode : public StreamNode //-------------------------------- OUT_ELEMENT
{
    Q_OBJECT
    Q_CLASSINFO ( "DefaultProperty", "forks" )

    Q_PROPERTY  ( int offset READ offset WRITE set_offset )
    Q_PROPERTY  ( qreal level READ level WRITE set_level )
    Q_PROPERTY  ( int numOutputs READ n_outputs WRITE setn_outputs )
    Q_PROPERTY  ( QQmlListProperty<Fork> forks READ forks )

    public: // ------------------------------------------------------
    QQmlListProperty<Fork> forks();
    QList<Fork*> get_forks() const ;

    virtual float level         ( ) const;
    virtual uint16_t offset     ( ) const;
    virtual void set_level      ( float );
    virtual void set_offset     ( uint16_t );

    virtual uint16_t n_outputs () const;
    virtual void setn_outputs ( uint16_t );

    protected: // --------------------------------------------------
    uint16_t    m_n_outputs;
    float       m_level;
    uint16_t    m_offset;

    QList<Fork*> m_forks;

};

class IOStreamNode : public InStreamNode, public OutStreamNode // --------- IN_OUT_ELEMENT
{
    Q_OBJECT
    Q_CLASSINFO     ( "DefaultProperty", "inputs" )

    Q_PROPERTY  ( bool bypassed READ bypassed WRITE set_bypassed )

    public: //-----------------------------------------------------
    bool bypassed() const;
    void set_bypassed(bool);

    private: //----------------------------------------------------
    bool m_bypassed;
};

class Fork : public StreamNode, public QQmlParserStatus //----------- SEND_ELEMENT
{
    Q_OBJECT

    Q_PROPERTY  ( InStreamNode* target READ target WRITE set_target )
    Q_PROPERTY  ( bool prefader READ prefader WRITE set_prefader )
    Q_PROPERTY  ( bool postfx READ postfx WRITE set_postfx )

    public: //--------------------------------------------------------------
    Fork  ( );
    ~Fork ( );

    virtual void classBegin ( ) override;
    virtual void componentComplete ( ) override;
    virtual void process ( float **&, const uint16_t nsamples ) override;

    bool prefader ( ) const;
    bool postfx   ( ) const;

    void set_prefader ( bool );
    void set_postfx   ( bool );

    OutStreamNode* const& from     ( ) const;
    InStreamNode*  const& to       ( ) const;

    void set_target ( InStreamNode* target );

    private: //--------------------------------------------------------------
    bool m_prefader;
    bool m_postfx;

    OutStreamNode*  m_emitter;
    InStreamNode*   m_target;
};

class Stream : protected StreamNode
{
    friend class StreamMaker;
    friend class WorldStream;
    // a stream is a chain of nodes including a source and an outfall
    // stream nodes may be included in multiple streams
    // as they can be sources and end for various streams

    // but a stream in itself can only be included in the WorldStream
    // and cannot embed other streams, only stream segments

    Q_OBJECT

    public: //--------------------------------------------------------
    const StreamNode& upstream_node ( );
    const StreamNode& downstream_node ( );

    static bool begins_with (const StreamNode& node);
    virtual void process ( float **&, const uint16_t nsamples ) override;

    void resolve ( );

    protected: //---------------------------------------------------------
    Stream ( );
    Stream ( StreamSegment const& );
    ~Stream ( );

    bool processed;
    QVector<StreamSegment*> m_segments;
};

class StreamSegment : public Stream, public IOStreamNode //-------------------------------------- STREAM_SEG
{
    friend class StreamMaker;

    // a stream segment is the smallest chain of nodes that can be processed with only one pool
    // which gets passed from element to element, as a stream
    // streams are only implicit, as they are non-instantiable in qml
    // a stream can have multiple "up" streams that pour themsleves in, or none...
    // it can also have multiple "down" streams, or none...

    Q_OBJECT

    public: //--------------------------------------------------------------
    virtual void process ( float **&, const uint16_t nsamples ) override;

    protected: //-----------------------------------------------------------
    StreamSegment(const StreamNode& outfall);
    ~StreamSegment();

    void alloc_pool ( const uint16_t nsamples );

    QVector<StreamSegment*> const& upstream_segments ( );
    QVector<StreamSegment*> const& downstream_segments ( );

    bool has_upstreams;
    bool has_downstreams;
    uint16_t max_channels;
    float** m_pool;

    private:
    void parse_upstream(const StreamNode& outfall);
    void parse_upstream(const StreamSegment& segment);

};

class StreamMaker //----------------------------------------------------- STREAM_MAKER
{
    friend class WorldStream;

    public:
    StreamMaker();
    void upstream ( const StreamNode& outfall );
    void resolve_streams ( );

    private:
    void upstream (const StreamSegment& segment );
    QVector<StreamSegment*> m_segments;
    QVector<Stream*> m_streams;
};

//---------------------------------------------------------------------------------------------------

class WorldStream : public IOStreamNode, public QQmlParserStatus, public QIODevice
{
    Q_OBJECT
    Q_INTERFACES    ( QQmlParserStatus QIODevice )
    Q_CLASSINFO     ( "DefaultProperty", "nodes" )

    // WorldStream is where all stream nodes dwell and are proccsed
    // singleton class

    Q_PROPERTY  ( QString device READ device WRITE set_device )
    Q_PROPERTY  ( int sampleRate READ samplerate WRITE set_samplerate )
    Q_PROPERTY  ( int blockSize READ blocksize WRITE set_blocksize )
    Q_PROPERTY  ( QQmlListProperty<StreamNode> nodes READ nodes )

    public: //------------------------------------------------------------
    static WorldStream& instance() { return m_singleton; }
    static Stream& WorldStream() { return *m_stream; }

    QQmlListProperty<StreamNode> nodes( );
    QList<StreamNode*> get_nodes( ) const;

    virtual void classBegin ( )                         override;
    virtual void componentComplete ( )                  override;

    virtual qint64 readData  ( char*, qint64 )          override;
    virtual qint64 writeData ( const char*, qint64 )    override;
    virtual qint64 bytesAvailable ()                    const override;

    virtual void process ( float **&, const uint16_t nsamples ) override;
    virtual void set_active(bool) override;

    uint32_t samplerate ( ) const;
    uint16_t blocksize  ( ) const;
    QString  device     ( ) const;

    void set_samplerate     ( uint32_t );
    void set_blocksize      ( uint16_t );
    void set_device         ( QString );

    protected slots: //---------------------------------------------------
    void onAudioStateChanged(QAudio::State);

    private: //-----------------------------------------------------------
    WorldStream  ( );
    ~WorldStream ( );

    static WorldStream  m_singleton;

    void            configure();
    uint16_t        m_blocksize;
    uint32_t        m_samplerate;
    QString         m_device;
    QAudioFormat    m_format;
    QAudioOutput*   m_output;
    float*          m_intl_buffer;

    QList<StreamNode*>   m_nodes;
    QVector<Stream>      m_streams;

};

#endif // BACKEND2_HPP

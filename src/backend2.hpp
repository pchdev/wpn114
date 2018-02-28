#ifndef BACKEND2_HPP
#define BACKEND2_HPP

#include <cstdint>
#include <vector>
#include "audiobackend.hpp"
#include "audiomaster.hpp"

using namespace std;

class WAElement : public QObject, public QQmlParserStatus //-------- ABSTRACT_AUDIO_ELEMENT
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

    protected: //---------------------------------------------------
    bool        m_active;
    bool        m_muted;

#define ON_COMPONENT_COMPLETED(obj)                         \
    void obj::classBegin() {}                               \
    void obj::componentComplete()

};

class WAIElement : public WAElement //-------------------------------- IN_ELEMENT
{
    Q_OBJECT
    Q_CLASSINFO     ( "DefaultProperty", "inputs" )

    Q_PROPERTY  ( int numInputs READ n_inputs WRITE setn_inputs )
    Q_PROPERTY  ( QQmlListProperty<WAOElement> inputs READ inputs )

    public: // ------------------------------------------------------
    QQmlListProperty<WAOElement> inputs();

    virtual uint16_t n_inputs() const;
    virtual void setn_inputs(uint16_t);

    private: //------------------------------------------------------
    QList<WAOElement*> m_inputs;
    uint16_t m_n_inputs;

};

class WAOElement : public WAElement //-------------------------------- OUT_ELEMENT
{
    Q_OBJECT
    Q_CLASSINFO ( "DefaultProperty", "sends" )

    Q_PROPERTY  ( int offset READ offset WRITE set_offset )
    Q_PROPERTY  ( qreal level READ level WRITE set_level )
    Q_PROPERTY  ( int numOutputs READ n_outputs WRITE setn_outputs )
    Q_PROPERTY  ( QQmlListProperty<WASndElement> sends READ sends )

    public: // ------------------------------------------------------
    QQmlListProperty<WASndElement> sends();
    QList<WASndElement*> get_sends() const ;

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

    QList<WASndElement*> m_sends;

};

class WAIOElement : public WAIElement, public WAOElement // --------- IN_OUT_ELEMENT
{
    Q_OBJECT
    Q_CLASSINFO     ( "DefaultProperty", "inputs" )

    Q_PROPERTY  ( bool bypassed READ bypassed WRITE set_bypassed )

    public: //---------------------------------------------------------
    bool bypassed() const;
    void set_bypassed(bool);

    private: //--------------------------------------------------------
    bool m_bypassed;
};

class WASndElement : public WAElement, public QQmlParserStatus //--- SEND_ELEMENT
{
    Q_OBJECT

    Q_PROPERTY  ( WAIElement* target READ target WRITE set_target )
    Q_PROPERTY  ( bool prefader READ prefader WRITE set_prefader )
    Q_PROPERTY  ( bool postfx READ postfx WRITE set_postfx )

    public: //--------------------------------------------------------------
    WASndElement();
    ~WASndElement();

    virtual void classBegin ( ) override;
    virtual void componentComplete ( ) override;
    virtual void process ( float **&, const uint16_t nsamples ) override;

    bool prefader ( ) const;
    bool postfx   ( ) const;

    void set_prefader ( bool );
    void set_postfx   ( bool );

    WAIElement* target  ( ) const;
    void set_target     ( WAIElement* target );

    private: //--------------------------------------------------------------
    bool m_prefader;
    bool m_postfx;

    WAIElement*  m_target;
};

class WAStream : public WAIOElement //------------------------------------------- STREAM
{
    Q_OBJECT
    // stream embeds multiple IO elements and has only one pool,
    // which gets passed from element to element, as a stream
    // streams are only implicit, as they are not instantiable in qml

    public: //----------------------------------------------
    WAStream(WAElement* const& outfall);
    ~WAStream();

    void pour ( const WAStream& stream ) const;
    const WAElement& first();

    virtual void classBegin ( ) override;
    virtual void componentComplete ( ) override;

    virtual void process ( float **&, const uint16_t nsamples ) override;

    private: //---------------------------------------------
    void alloc_pool ( const uint16_t nsamples );

    QVector<WAElement*> m_aelements;
    // the IO elements that compose the stream
    // note that the sources can or cannot be included

    // i.e. if a WAElement has multiple stream targets to pour itself in
    // then it becomes a stream in itself

    QVector<WAStream*> m_upstreams;
    QVector<WAStream*> m_downstreams;
    // the descending streams in which this one has to pour itself in

    float** m_pool;
};

class WAStreamFactory
{
    public:
    WAStreamFactory();
    QVector<WAStream*> upstream ( const WAElement& outfall );
    QVector<WAStream*> upstream ( const WAStream& stream );

};

//---------------------------------------------------------------------------------------------------

class World : public WAIOElement, public QQmlParserStatus, public QIODevice
{
    Q_OBJECT
    Q_INTERFACES    ( QQmlParserStatus QIODevice )
    Q_CLASSINFO     ( "DefaultProperty", "aelements" )

    // World is where all aelements dwell, and are processed
    // singleton class

    Q_PROPERTY  ( QString device READ device WRITE set_device )
    Q_PROPERTY  ( int sampleRate READ samplerate WRITE set_samplerate )
    Q_PROPERTY  ( int blockSize READ blocksize WRITE set_blocksize )
    Q_PROPERTY  ( QQmlListProperty<WAElement> aelements READ aelements )

    public: //------------------------------------------------------------
    static World& instance();

    QQmlListProperty<WAElement> aelements( );
    QList<WAElement*> get_aelements( ) const;

    virtual void classBegin ( )                         override;
    virtual void componentComplete ( )                  override;

    virtual qint64 readData  ( char*, qint64 )          override;
    virtual qint64 writeData ( const char*, qint64 )    override;
    virtual qint64 bytesAvailable ()                    const override;

    virtual void process ( float **&, const uint16_t nsamples ) override;

    Q_INVOKABLE void run  ( );
    Q_INVOKABLE void stop ( );

    uint32_t samplerate ( ) const;
    uint16_t blocksize  ( ) const;
    QString  device     ( ) const;

    void set_samplerate     ( uint32_t );
    void set_blocksize      ( uint16_t );
    void set_device         ( QString );

    protected slots: //---------------------------------------------------
    void onAudioStateChanged(QAudio::State);

    private: //-----------------------------------------------------------
    World  ( );
    ~World ( );

    static World    m_singleton;

    void            configure();
    uint16_t        m_blocksize;
    uint32_t        m_samplerate;
    QString         m_device;
    QAudioFormat    m_format;
    QAudioOutput*   m_output;
    float*          m_intl_buffer;

    QVector<WAStream*>  m_streams;
    QList<WAElement*>   m_aelements;

};

#endif // BACKEND2_HPP

#ifndef BACKEND2_HPP
#define BACKEND2_HPP

#include <cstdint>
#include <vector>
#include "audiobackend.hpp"
#include "audiomaster.hpp"

using namespace std;
namespace wpn114 {
namespace audio {

typedef void ( AElement::*fnproc_t )( float**&, const uint16_t );

struct pnode
{
    float**   src;
    fnproc_t  prc;
    uint16_t  nin;
    uint16_t  nout;
    uint16_t  off;
    vector<stream*>  streams;
};

struct stream : pnode
{
    vector<pnode*>   pnodes;
};

class AAbstractElement : public QObject
{
    Q_OBJECT

    Q_PROPERTY  ( int offset READ offset WRITE set_offset )
    Q_PROPERTY  ( qreal level READ level WRITE set_level )
    Q_PROPERTY  ( bool active READ active WRITE set_active )
    Q_PROPERTY  ( bool muted READ muted WRITE set_muted )

    public: // -----------------------------------------------
    virtual float level () const;
    virtual uint16_t offset () const;
    virtual bool active () const;
    virtual bool muted () const;

    virtual void set_active     ( bool );
    virtual void set_muted      ( bool );
    virtual void set_level      ( float );
    virtual void set_offset     ( uint16_t );

    protected: //-------------------------------------------
    bool        m_active;
    bool        m_muted;
    float       m_level;
    uint16_t    m_offset;

};

class AElement : public AAbstractElement //-------------------------- GENERATOR_ELEMENT
{
    Q_OBJECT
    Q_CLASSINFO     ( "DefaultProperty", "sends" )

    Q_PROPERTY  ( int numOutputs READ n_outputs WRITE setn_outputs )
    Q_PROPERTY  ( QQmlListProperty<ASendElement> sends READ sends )

    public: // ------------------------------------------------------
    ~AElement();
    QQmlListProperty<ASendElement> sends();

    virtual float process (float**&, const uint16_t nsamples) = 0;

    uint16_t n_outputs () const;
    virtual void setn_outputs ( uint16_t );

    protected: // --------------------------------------------------
    uint16_t    m_n_outputs;
    QList<ASendElement*> m_sends;

#define ON_COMPONENT_COMPLETED(obj)                         \
    void obj::classBegin() {}                               \
    void obj::componentComplete()
};

class AProcessorElement : public AElement // ---------------------- PROCESSOR_ELEMENT
{
    Q_OBJECT
    Q_CLASSINFO     ( "DefaultProperty", "inputs" )

    Q_PROPERTY  ( int numInputs READ n_inputs WRITE setn_inputs )
    Q_PROPERTY  ( QQmlListProperty<AElement> inputs READ inputs )

    public: //-----------------------------------------------------
    ~AProcessorElement();
    QQmlListProperty<AElement> inputs();

    uint16_t n_inputs() const;
    void setn_inputs(uint16_t);

    private: //---------------------------------------------------
    QList<AElement*> m_inputs;
    uint16_t m_n_inputs;
};

class ASendElement : public AAbstractElement, public QQmlParserStatus //--- SEND_ELEMENT
{
    Q_OBJECT

    Q_PROPERTY  ( AProcessorElement* target READ target WRITE set_target )
    Q_PROPERTY  ( bool prefader READ prefader WRITE set_prefader )
    Q_PROPERTY  ( bool postfx READ postfx WRITE set_postfx )

    public: //--------------------------------------------------------------
    ASendElement();
    ~ASendElement();

    virtual void componentComplete  () override;
    virtual void classBegin         () override;

    bool        prefader    () const;
    bool        postfx      () const;

    AProcessorElement* target () const;

    void set_prefader   ( bool );
    void set_postfx     ( bool );
    void set_target     ( AProcessorElement* target );

    private: //--------------------------------------------------------------
    bool        m_prefader;
    bool        m_postfx;

    AProcessorElement* m_target;
};

class World
{
    public:
    void  parse   ( );

    void  genreg  ( const AElement& );
    void  procreg ( const AProcessorElement& );
    //    these two funfctions have to be called once the qml element is complete

    void      alloc   ( const uint16_t blocksize );

    float**&  run     ( const uint16_t nsamples );

    stream*             mainstream ( ) const;
    vector<pnode*>      prm_nodes  ( ) const;
    vector<stream*>     streams    ( ) const;

    static World&       instance        ( );

    uint32_t     samplerate      ( ) const;
    uint16_t     blocksize       ( ) const;
    void         set_samplerate  ( uint32_t );
    void         set_blocksize   ( uint16_t );

//---------------------------------------------------------------------------------------------------
#define WORLD_RATE      World::instance().samplerate()
#define WORLD_BLOCK     World::instance().blocksize()

#define WORLD_GENERATOR_COMPONENT_REGISTRATION ( generator ) World::instance().genreg( generator )
#define WORLD_PROCESSOR_COMPONENT_REGISTRATION ( processor ) World::instance().procreg( processor )
#define WORLD_HYBRID_COMPONENT_REGISTRATION    ( hcomponent )
//---------------------------------------------------------------------------------------------------

    private: // -----------------------
    World  ( );
    ~World ( );

    static World        m_instance;
    stream*             m_mainstream;
    vector<pnode*>      m_prnodes;
    vector<stream*>     m_streams;
    uint32_t            m_samplerate;
    uint16_t            m_blocksize;
};

//---------------------------------------------------------------------------------------------------

class WorldInterface : public AAbstractElement, public QQmlParserStatus, public QIODevice
{
    Q_OBJECT
    Q_INTERFACES    ( QQmlParserStatus )
    Q_CLASSINFO     ( "DefaultProperty", "aelements" )

    Q_PROPERTY  ( QString device READ device WRITE set_device )
    Q_PROPERTY  ( int numInputs READ n_inputs WRITE setn_inputs )
    Q_PROPERTY  ( int numOutputs READ n_outputs WRITE setn_outputs )
    Q_PROPERTY  ( int sampleRate READ samplerate WRITE set_samplerate )
    Q_PROPERTY  ( int blockSize READ blocksize WRITE set_blocksize )
    Q_PROPERTY  ( QQmlListProperty<AObject> aelements READ aelements )

    public:
    WorldInterface  ( );
    ~WorldInterface ( );

    QQmlListProperty<AElement> aelements( );

    virtual void classBegin ( )                         override;
    virtual void componentComplete ( )                  override;
    virtual qint64 readData  ( char*, qint64 )          override;
    virtual qint64 writeData ( const char*, qint64 )    override;
    virtual qint64 bytesAvailable ()                    const override;

    Q_INVOKABLE void run  ( );
    Q_INVOKABLE void stop ( );

    uint32_t samplerate ( ) const;
    uint16_t blocksize  ( ) const;
    uint16_t n_inputs   ( ) const;
    uint16_t n_outputs  ( ) const;
    QString  device     ( ) const;

    void set_samplerate     ( uint32_t );
    void set_blocksize      ( uint16_t );
    void setn_inputs        ( uint16_t );
    void setn_outputs       ( uint16_t );
    void set_device         ( QString );

    protected slots: //---------------------
    void onAudioStateChanged(QAudio::State);

    private: //-----------------------------
    void            configure();
    uint16_t        m_n_inputs;
    uint16_t        m_n_outputs;
    QString         m_device;
    QAudioFormat    m_format;
    QAudioOutput*   m_output;
    float*          m_buffer;

    QList<AElement*> m_aelements;

};

}
}
}

#endif // BACKEND2_HPP

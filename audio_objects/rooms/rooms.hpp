#ifndef ROOMS_H
#define ROOMS_H

#include <src/audiobackend.hpp>
#include <array>

class RoomsObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY      ( float level READ level WRITE setLevel )
    Q_PROPERTY      ( QVector<qreal> positions READ positions WRITE setPositions )
    Q_PROPERTY      ( float influence READ influence WRITE setInfluence )

public:
    RoomsObject();
    ~RoomsObject();

    QVector<qreal> positions() const;
    void setPositions( const QVector<qreal> );

    float level         ( ) const;
    float influence     ( ) const;
    void setLevel       ( const float );
    void setInfluence   ( const float );

protected:
    float               m_level;
    float               m_influence;
    QVector<qreal>      m_positions;

};

class SpeakerObject : public RoomsObject
{
    Q_OBJECT
    Q_PROPERTY      ( int output READ output WRITE setOutput )

public:

    virtual quint16 nspeakers() const = 0;

    enum Axis   { X, Y, BOTH }; Q_ENUM ( Axis )
    enum Order  { Clockwise, Anticlockwise, Pairing }; Q_ENUM ( Order )

    int     output() const;
    void    setOutput(const int);

private:
    quint16 m_output;
};

class Speaker : public SpeakerObject
{
    Q_OBJECT

public:
    Speaker();
    virtual quint16 nspeakers() const { return 1; }

};

class SpeakerPair : public Speaker, public QQmlParserStatus
{
    Q_OBJECT

    Q_INTERFACES    ( QQmlParserStatus )
    Q_PROPERTY      ( qreal offset READ offset WRITE setOffset )
    Q_PROPERTY      ( QVector<qreal> offsets READ offsets WRITE setOffsets )
    Q_PROPERTY      ( Axis axis READ axis WRITE setAxis )

public:
    SpeakerPair();

    virtual quint16 nspeakers() const { return 2; }

    virtual void classBegin()           override;
    virtual void componentComplete()    override;

    Axis            axis        () const;
    qreal           offset      () const;
    QVector<qreal>  offsets     () const;

    void setOffset      ( const qreal );
    void setOffsets     ( const QVector<qreal> );
    void setAxis        ( const Axis );

private:
    Axis            m_axis;
    qreal           m_offset;
    QVector<qreal>  m_offsets;

};

class SpeakerRing : public Speaker, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES    ( QQmlParserStatus )
    Q_PROPERTY      ( qreal offset READ offset WRITE setOffset )
    Q_PROPERTY      ( Order order READ order WRITE setOrder )
    Q_PROPERTY      ( int nspeakers READ nspeakers WRITE setNspeakers )

public:
    SpeakerRing();

    virtual quint16 nspeakers() const { return m_nspeakers; }

    virtual void classBegin()           override;
    virtual void componentComplete()    override;

    qreal   offset      () const;
    Order   order       () const;

    void setOffset      ( const qreal );
    void setOrder       ( const Order );
    void setNspeakers   ( const quint16 );

private:
    qreal       m_offset;
    Order       m_order;
    quint16     m_nspeakers;
};

class Source : public RoomsObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY      ( QQmlListProperty<AudioObject> inputs READ inputs )
    Q_PROPERTY      ( int nchannels READ nchannels )
    Q_INTERFACES    ( QQmlParserStatus )
    Q_CLASSINFO     ( "DefaultProperty", "inputs" )

public:

    virtual void classBegin()           override;
    virtual void componentComplete()    override;

    quint16 nchannels() const;

    QQmlListProperty<AudioObject> inputs();
    QList<AudioObject*> get_inputs();

private:
    QList<AudioObject*> m_inputs;
    quint16 m_nchannels;

};

// qt quick gui todo
class RoomsSetup : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES    ( QQmlParserStatus )
    Q_PROPERTY      ( int numOutputs READ numOutputs )
    Q_PROPERTY      ( QQmlListProperty<SpeakerObject> speakers READ speakers )
    Q_CLASSINFO     ( "DefaultProperty", "speakers" )

public:

    struct SpeakerChannel
    {
        qreal x;
        qreal y;
        qreal r;
        qreal l;
    };

    RoomsSetup();
    ~RoomsSetup();

    virtual void classBegin()           override;
    virtual void componentComplete()    override;

    QQmlListProperty<SpeakerObject> speakers();
    QVector<SpeakerChannel> get_channels() const;
    QList<SpeakerObject*>& get_speakers();
    void appendSpeaker(SpeakerObject*);

    uint16_t numOutputs() const;

private:    
    quint16 m_noutputs;
    QList<SpeakerObject*> m_speakers;
    QVector<SpeakerChannel> m_channels;
    static void appendSpeaker( QQmlListProperty<SpeakerObject>*, SpeakerObject* );

};

class Rooms : public AudioEffectObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES    ( QQmlParserStatus )
    Q_PROPERTY      ( RoomsSetup* setup READ setup WRITE setSetup )
    Q_PROPERTY      ( QQmlListProperty<Source> sources READ sources )
    Q_CLASSINFO     ( "DefaultProperty", "sources" )

public:
    Rooms();
    ~Rooms();

    virtual float** process( const quint16 nsamples ) override;

    virtual void classBegin         () override;
    virtual void componentComplete  () override;

    QQmlListProperty<Source> sources();

    RoomsSetup* setup   () const;
    void setSetup       (RoomsSetup*);

protected:    
    float**& get_inputs( const quint64 nsamples ); // override

private:
    RoomsSetup*     m_setup;
    QList<Source*>  m_sources;
};

#endif // ROOMS_H

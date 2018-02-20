#ifndef ROOMS_H
#define ROOMS_H

#include "audiobackend.h"
#include <array>

class RoomsChannel : public QObject
{
    Q_OBJECT
    Q_PROPERTY      ( float level READ level WRITE setLevel )
    Q_PROPERTY      ( QVector2D position READ position WRITE setPosition )
    Q_PROPERTY      ( float influence READ influence WRITE setInfluence )

public:
    RoomsChannel();
    ~RoomsChannel();

    float       level       () const;
    float       influence   () const;
    QVector2D   position    () const;

    void setLevel       ( const float );
    void setInfluence   ( const float );
    void setPosition    ( const QVector2D );

protected:
    float           m_level;
    float           m_influence;
    QVector2D       m_position;

};

class Speaker : public RoomsChannel
{
    Q_OBJECT
    Q_PROPERTY      ( int output READ output WRITE setOutput )

public:
    int output() const;
    void setOutput(const int);

private:
    quint16 m_output;
};

class Source : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY      ( QQmlListProperty<AudioObject> inputs READ inputs )
    Q_PROPERTY      ( int nchannels READ nchannels )
    Q_PROPERTY      ( QVector<qreal> positions READ positions WRITE setPositions)
    Q_PROPERTY      ( qreal influence READ influence WRITE setInfluence)
    Q_PROPERTY      ( qreal level READ level WRITE setLevel )

    Q_INTERFACES    ( QQmlParserStatus )

    Q_CLASSINFO     ( "DefaultProperty", "inputs" )

public:

    void classBegin() override;
    void componentComplete() override;

    quint16 nchannels() const;

    QVector<qreal> positions() const;
    qreal influence() const;
    qreal level () const;

    void setPositions(const QVector<qreal>);
    void setInfluence(const qreal);
    void setLevel(const qreal);

    QQmlListProperty<AudioObject> inputs();
    QList<AudioObject*> get_inputs();

private:
    QList<AudioObject*> m_inputs;
    QVector<qreal> m_positions;
    qreal m_influence;
    qreal m_level;
    quint16 m_nchannels;

};

// qt quick gui todo
class RoomsSetup : public QObject
{
    Q_OBJECT
    Q_PROPERTY      ( int numOutputs READ numOutputs )
    Q_PROPERTY      ( QQmlListProperty<RoomsChannel> speakers READ speakers )
    Q_CLASSINFO     ( "DefaultProperty", "speakers" )

public:
    RoomsSetup();
    ~RoomsSetup();

    QQmlListProperty<RoomsChannel> speakers();
    QList<RoomsChannel *>& get_speakers();

    uint16_t numOutputs() const;

private:
    quint16                 m_noutputs;
    QList<RoomsChannel*>    m_speakers;
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
    RoomsSetup*             m_setup;
    QList<Source*>          m_sources;
};

#endif // ROOMS_H

#ifndef ROOMS_H
#define ROOMS_H

#include "audiobackend.h"

class RoomsElement : public QObject
{
    Q_OBJECT
    Q_PROPERTY      ( float level READ level WRITE setLevel )
    Q_PROPERTY      ( QVector2D position READ position WRITE setPosition )
    Q_PROPERTY      ( float influence READ influence WRITE setInfluence )

public:
    RoomsElement();
    ~RoomsElement();

    float       level       () const;
    float       influence   () const;
    QVector2D   position    () const;

    void setLevel       ( const float );
    void setInfluence   ( const float );
    void setPosition    ( const QVector2D );

protected:
    float       m_level;
    float       m_influence;
    QVector2D   m_position;

};

class Speaker : public RoomsElement {};

class Source : public RoomsElement
{
    Q_PROPERTY      ( QQmlListProperty<AudioObject> inputs READ inputs )
    Q_PROPERTY      ( QVector2D lposition READ lposition WRITE setLposition )
    Q_PROPERTY      ( QVector2D rposition READ rposition WRITE setRposition )
    Q_PROPERTY      ( QQmlListProperty<AudioObject> inputs READ inputs )

    Q_CLASSINFO     ( "DefaultProperty", "inputs" )

public:
    QQmlListProperty<AudioObject> inputs();
    QList<AudioObject*> get_inputs();

    QVector2D lposition() const;
    QVector2D rposition() const;

    void setLposition(const QVector2D);
    void setRPosition(const QVector2D);

private:
    QList<AudioObject*> m_inputs;
    QVector2D m_lposition;
    QVector2D m_rposition;

};


// qt quick gui todo
class RoomsSetup : public QObject
{
    Q_OBJECT
    Q_PROPERTY      ( int numOutputs READ numOutputs )
    Q_PROPERTY      ( QQmlListProperty<RoomsElement> speakers READ speakers )
    Q_CLASSINFO     ( "DefaultProperty", "speakers" )

public:
    RoomsSetup();
    ~RoomsSetup();

    QQmlListProperty<RoomsElement> speakers();
    QList<RoomsElement*> get_speakers();

    uint16_t numOutputs() const;

private:
    quint16                 m_noutputs;
    QList<RoomsElement*>    m_speakers;
};


class Rooms : public AudioEffectObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES    ( QQmlParserStatus )
    Q_PROPERTY      ( RoomsSetup setup READ setup WRITE setSetup )
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
    float**& get_inputs(const quint64 nsamples); // override

private:
    RoomsSetup*             m_setup;
    QList<Source*>          m_sources;
    QList<float*[4]>        m_src_data;
};

#endif // ROOMS_H

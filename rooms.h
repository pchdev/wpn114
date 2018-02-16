#ifndef ROOMS_H
#define ROOMS_H

#include "audiobackend.h"

// qt quick gui todo
class RoomsSetup : public QObject
{
    Q_OBJECT
    Q_PROPERTY ( int numInputs READ numInputs WRITE setNumInputs NOTIFY numInputsChanged )
    Q_PROPERTY ( int numOutputs READ numOutputs WRITE setNumOutputs NOTIFY numOutputsChanged )

public:
    struct r_obj
    {
        float x;
        float y;
        float r;
        float l;
    };

    RoomsSetup              ();
    ~RoomsSetup             ();

    uint16_t numInputs      () const;
    uint16_t numOutputs     () const;

    void setNumInputs       (const uint16_t);
    void setNumOutputs      (const uint16_t);

signals:
    void speakerPosChanged      (quint16 index);
    void numInputsChanged       ();
    void numOutputsChanged      ();

private:
    quint16                 m_ninputs;
    quint16                 m_noutputs;
    std::vector<r_obj>      m_objects;
};

class Rooms : public AudioObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES    ( QQmlParserStatus )
    //Q_PROPERTY      ( RoomsSetup setup READ setup WRITE setSetup )

public:
    Rooms();
    ~Rooms();

    virtual float** process         (const quint16 nsamples);
    virtual void    classBegin      ();
    virtual void componentComplete  ();

    RoomsSetup* setup   () const;
    void setSetup       (RoomsSetup*);

private:
    RoomsSetup* m_setup;
};

#endif // ROOMS_H

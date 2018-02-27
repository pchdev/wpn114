#ifndef AUDIOMASTER_H
#define AUDIOMASTER_H

#include <QObject>
#include <QIODevice>
#include <src/audiobackend.hpp>

class AudioObject;

class AudioMaster : public QIODevice
{    
    Q_OBJECT
    Q_PROPERTY  ( float level READ level WRITE setLevel )
    Q_PROPERTY  ( bool active READ active WRITE setActive )
    Q_PROPERTY  ( bool muted READ muted WRITE setMuted )
    Q_PROPERTY  ( quint16 numInputs READ numInputs WRITE setNumInputs )
    Q_PROPERTY  ( quint16 numOutputs READ numOutputs WRITE setNumOutputs )
    Q_PROPERTY  ( QQmlListProperty<AudioObject> units READ units )
    Q_CLASSINFO ( "DefaultProperty", "units")

public:
    AudioMaster();
    ~AudioMaster();

    QQmlListProperty<AudioObject> units() ;
    QList<AudioObject*> get_units();

    float       level           () const;
    bool        active          () const;
    bool        muted           () const;
    quint16     numInputs()     const;
    quint16     numOutputs()    const;

    void setNumInputs           (const quint16);
    void setNumOutputs          (const quint16);
    void setLevel               (const float);
    void setActive              (const bool);
    void setMuted               (const bool);

    qint64 readData             (char *data, qint64 maxlen) override;
    qint64 writeData            (const char *data, qint64 len) override;
    qint64 bytesAvailable       () const override;

protected:
    bool                    m_active;
    bool                    m_muted;
    float*                  m_buf;
    float                   m_level;
    quint16                 m_num_inputs;
    quint16                 m_num_outputs;
    QList<AudioObject*>     m_units;

};

#endif // AUDIOMASTER_H

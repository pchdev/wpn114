#ifndef FIELDS_H
#define FIELDS_H

#include <QAudioBuffer>
#include <QAudioDecoder>
#include <src/wpnsndfile.hpp>
#include <src/audiobackend.hpp>

#define ENVSIZE 16384

class Fields : public AudioObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES    ( QQmlParserStatus )
    Q_PROPERTY      ( int xfade READ xfade WRITE setXfade )
    Q_PROPERTY      ( QString path READ path WRITE setPath NOTIFY pathChanged )

public:    
    Fields                      ();
    ~Fields                     ();

    float** process             (const quint16 nsamples) override;
    void classBegin             () override;
    void componentComplete      () override;

    uint32_t xfade              () const;
    QString path                () const;

    void setXfade               (const uint32_t);
    void setPath                (const QString);
    void setNumInputs           (const quint16);

signals:
    void pathChanged();
    void bufferReady();

private:
    QString             m_path;
    uint32_t            m_xfade;
    float               m_env[ENVSIZE];
    quint64             m_epos;
    quint64             m_spos;
    quint64             m_xfade_point;
    float               m_env_incr;
    sndbuf*             m_buf;
};

#endif // FIELDS_H

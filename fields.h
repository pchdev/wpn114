#ifndef FIELDS_H
#define FIELDS_H

#include "audiobackend.h"
#include <QAudioBuffer>
#include <QAudioDecoder>

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
    QDataStream*        m_stream;
    QFile*              m_file;
    QAudioBuffer*       m_buffer;
    float               m_env[ENVSIZE];
    quint32             m_spos;
    quint32             m_bpos;
};

#endif // FIELDS_H

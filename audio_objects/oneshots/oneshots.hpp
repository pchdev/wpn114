#ifndef ONESHOTS_H
#define ONESHOTS_H

#include <src/audiobackend.hpp>
#include <src/wpnsndfile.hpp>

class Oneshots : public AudioObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES ( QQmlParserStatus )
    Q_PROPERTY ( QString path READ path WRITE setPath NOTIFY pathChanged )

public:
    Oneshots();
    ~Oneshots();

    void classBegin() override;
    void componentComplete() override;
    float** process(const quint16 nsamples) override;

    QString path() const;
    void setPath(const QString);

signals:
    void pathChanged();
    void bufferLoaded();
    void playbackEnd();

private:
    QString     m_path;
    sndbuf*     m_buf;
    quint64     m_pos;
};

#endif // ONESHOTS_H

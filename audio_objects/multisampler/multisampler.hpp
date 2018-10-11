#ifndef MULTISAMPLER_HPP
#define MULTISAMPLER_HPP

#include <src/audio/audio.hpp>
#include <audio_objects/sampler/sampler.hpp>
#include <QDir>

class MultiSampler : public StreamNode
{
    Q_OBJECT

    Q_PROPERTY  ( QString path READ path WRITE setPath )
    Q_PROPERTY  ( QStringList files READ files )

    public:
    MultiSampler();   

    virtual void userInitialize(qint64) override;
    virtual float** userProcess(float** buf, qint64 nsamples) override;

    Q_INVOKABLE void play(quint16 index);
    Q_INVOKABLE void stop(quint16 index);

    QString path() const { return m_path; }
    void setPath(QString path);

    QStringList files() const { return m_files; }

    private:
    QDir* m_dir;
    QString m_path;
    QStringList m_files;
    QVector<Sampler*> m_samplers;
};

#endif // MULTISAMPLER_HPP

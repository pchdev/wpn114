#ifndef MULTISAMPLER_HPP
#define MULTISAMPLER_HPP

#include <src/audio/audio.hpp>
#include <audio_objects/sampler/sampler.hpp>
#include <QDir>

class MultiSampler : public StreamNode
{
    Q_OBJECT

    Q_PROPERTY  ( QString path READ path WRITE setPath )
    Q_PROPERTY  ( QStringList files READ files NOTIFY filesChanged )

    public:
    MultiSampler();   

    virtual void initialize ( qint64 ) override;
    virtual float** process ( float** buf, qint64 nsamples ) override;
    virtual void expose(WPNNode*) override;

    Q_INVOKABLE void play(QVariant var);
    Q_INVOKABLE void playRandom();

    Q_INVOKABLE void stop(QVariant var);

    QString path    ( ) const { return m_path; }
    void setPath    ( QString path );

    QStringList files() const { return m_files; }

    signals:
    void filesChanged();

    private:
    QDir* m_dir;
    QString m_path;
    QStringList m_files;
    QVector<Sampler*> m_samplers;
};

#endif // MULTISAMPLER_HPP

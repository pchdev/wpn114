#ifndef DOWNMIX_HPP
#define DOWNMIX_HPP

#include <src/audio/audio.hpp>

class Downmix : public StreamNode
{
    Q_OBJECT
    Q_PROPERTY  ( QVariant channels READ channels WRITE setChannels )

    public:
    Downmix();

    virtual void initialize(qint64) override;
    virtual float** process(float**, qint64) override;

    QVariant channels() const;
    void setChannels(QVariant channels);

    private:
    QVector<quint16> m_channels;
};

#endif // DOWNMIX_HPP

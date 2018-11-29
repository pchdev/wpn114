#ifndef CHANNELMAPPER_HPP
#define CHANNELMAPPER_HPP

#include <source/audio/audio.hpp>

class ChannelMapper : public StreamNode
{
    Q_OBJECT
    Q_PROPERTY  ( QVariantList map READ map WRITE setMap )

    public:
    ChannelMapper();
    void initialize(qint64) override {};
    float** process(float**, qint64) override;

    QVariantList map() const;
    void setMap(QVariantList const map);

    private:
    QVector<quint16> m_map;

};

#endif // CHANNELMAPPER_HPP

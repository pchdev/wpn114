#include "bursts.hpp"

Bursts::Bursts()
{

}

void Bursts::setLength(quint32 length)
{
    m_length = length;
}

void Bursts::setChannels(QVariantList channels)
{
    auto vec = channels.toVector();

    for ( const auto& v : vec )
          m_channels << v.toInt();
}

QVariantList Bursts::channels() const
{
    QVariantList list;

    for ( const auto& ch : m_channels )
          list << ch;

    return list;
}

void Bursts::componentComplete()
{

}

void Bursts::initialize(qint64 nsamples)
{

}

float** Bursts::process(float** in, qint64 nsamples)
{
    return in;
}

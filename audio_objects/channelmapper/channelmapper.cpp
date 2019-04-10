#include "channelmapper.hpp"
#include <QtDebug>

ChannelMapper::ChannelMapper()
{

}

QVariantList ChannelMapper::map() const
{
    QVariantList list;

    for ( const auto& channel : m_map )
          list << channel;

    return list;
}

void ChannelMapper::setMap(QVariantList const map)
{
    QVector<quint16> tmp;
    for ( const auto& index : map )
          tmp << index.toInt();

    if (tmp.size() == m_map.size())
        return;

    m_map = tmp;
}

float** ChannelMapper::process(float** in, qint64 nsamples)
{
    auto nout = m_num_outputs;
    quint16 index = 0;
    auto out = m_out;
    StreamNode::resetBuffer(out, nout, nsamples);

    for ( const auto& channel : m_map ) {
        memcpy(out[channel], in[index], sizeof(float)*nsamples);
         ++index;
    }

    return out;
}

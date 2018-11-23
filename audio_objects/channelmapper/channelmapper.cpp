#include "channelmapper.hpp"

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
    for ( const auto& index : map )
          m_map << index.toInt();
}

float** ChannelMapper::process(float** in, qint64 nsamples)
{
    auto nout = m_num_outputs;
    quint16 index = 0;
    auto out = m_out;
    StreamNode::resetBuffer(out, nout, nsamples);

    for ( const auto& channel : m_map )
    {
         out[channel] = in[index];
         ++index;
    }

    return out;
}

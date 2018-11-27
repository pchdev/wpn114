#include "downmix.hpp"
#include <QtDebug>

Downmix::Downmix()
{
    SETTYPE ( StreamType::Effect );
}

QVariant Downmix::channels() const
{
    QVariant ret;

    if ( m_channels.size() == 1 )
         ret = m_channels[0];
    else
    {
        QVariantList list;
        for ( const auto& ch : m_channels )
            list << ch;

        ret = list;
    }

    return ret;
}

void Downmix::setChannels(QVariant channels)
{
    m_channels.clear();
    if ( channels.type() == QMetaType::Int )
        m_channels << channels.toInt();

    else
    {
        auto list = channels.toList();
        for ( const auto& v : list )
            m_channels << v.toInt();
    }
}

void Downmix::initialize(qint64 nsamples)
{

}

float** Downmix::process(float** in, qint64 nsamples)
{
    auto out   = m_out;
    auto nout  = m_num_outputs;
    auto nin   = m_num_inputs;

    StreamNode::resetBuffer(out, nout, nsamples);
    StreamNode::mergeBuffers(out, in, nout, nin, nsamples);

    for ( quint16 s = 0; s < nsamples; ++s )
        for ( quint16 ch = 0; ch < nout; ++ch )
            if ( !m_channels.contains(ch) )
                for ( const auto& tch : m_channels )
                    out[tch][s] += in[ch][s]*0.33;

    return out;
}

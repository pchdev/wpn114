#include "multisampler.hpp"

MultiSampler::MultiSampler() : m_dir(nullptr)
{
    SETN_IN     ( 0 );
    SETN_OUT    ( 2 );
}

void MultiSampler::setPath(QString path)
{
    m_path = path;
    if ( m_dir ) delete m_dir;

    m_dir = new QDir(path);
    m_dir->setNameFilters(QStringList{"*.wav"});

    m_files = m_dir->entryList();

    for ( const auto& file : m_files )
    {
        Sampler* sampler = new Sampler;

        sampler->setPath(m_path+"/"+file);
        sampler->componentComplete();
        sampler->setActive(true);
    }
}

void MultiSampler::initialize(qint64 nsamples)
{
    for ( const auto& sampler : m_samplers )
        sampler->initialize(nsamples);
}

float** MultiSampler::process(float** buf, qint64 nsamples)
{
    auto out = m_out;
    auto nout = m_num_outputs;

    for ( const auto& sampler : m_samplers )
        StreamNode::mergeBuffers( out, sampler->process(nullptr, nsamples),
                                 nout, sampler->numOutputs(), nsamples );

    return out;
}

void MultiSampler::play(quint16 index)
{
    m_samplers[index]->play();
}

void MultiSampler::stop(quint16 index)
{
    m_samplers[index]->stop();
}

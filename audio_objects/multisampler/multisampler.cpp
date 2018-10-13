#include "multisampler.hpp"
#include <cstdlib>

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

void MultiSampler::expose(WPNNode* node)
{
    auto funcs = m_exp_node->createSubnode("functions");
    auto play = funcs->createSubnode("play");
    auto stop = funcs->createSubnode("stop");
    auto rplay = funcs->createSubnode("rplay");

    play->setType(Type::Int);
    stop->setType(Type::Int);
    rplay->setType(Type::Impulse);

    QObject::connect(play, SIGNAL(valueReceived(QVariant)), this, SLOT(play(quint16)));
    QObject::connect(rplay, SIGNAL(valueReceived(QVariant)), this, SLOT(playRandom()));

    QObject::connect(stop, SIGNAL(valueReceived(QVariant)), this, SLOT(stop(quint16)));
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

void MultiSampler::playRandom()
{
    auto rand = std::rand()/RAND_MAX;
    quint16 srand = m_samplers.size()*(quint16)rand;

    play(srand);
}

void MultiSampler::stop(quint16 index)
{
    m_samplers[index]->stop();
}

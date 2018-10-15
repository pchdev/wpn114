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

    emit filesChanged();
}

void MultiSampler::initialize(qint64 nsamples)
{
    for ( const auto& sampler : m_samplers )
        sampler->initialize(nsamples);
}

void MultiSampler::expose(WPNNode* node)
{
    auto funcs      = m_exp_node->createSubnode("functions");
    auto play       = funcs->createSubnode("play");
    auto playstr    = funcs->createSubnode("playFile");
    auto stop       = funcs->createSubnode("stop");
    auto stopstr    = funcs->createSubnode("stopFile");
    auto rplay      = funcs->createSubnode("rplay");

    play->setType       ( Type::Int );
    stop->setType       ( Type::Int );
    playstr->setType    ( Type::String );
    stopstr->setType    ( Type::String );
    rplay->setType      ( Type::Impulse );

    QObject::connect(play,  SIGNAL(valueReceived(QVariant)), this, SLOT(play(QVariant)));
    QObject::connect(playstr,  SIGNAL(valueReceived(QVariant)), this, SLOT(play(QVariant)));
    QObject::connect(rplay, SIGNAL(valueReceived(QVariant)), this, SLOT(playRandom()));
    QObject::connect(stop,  SIGNAL(valueReceived(QVariant)), this, SLOT(stop(QVariant)));
    QObject::connect(stopstr,  SIGNAL(valueReceived(QVariant)), this, SLOT(stop(QVariant)));
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

void MultiSampler::play(QVariant variant)
{
    if ( variant.type() == QMetaType::Int )
        m_samplers[variant.toInt()]->play();

    else if ( variant.type() == QMetaType::QString )
    {
        auto idx = m_files.indexOf(variant.toString());
        if ( idx < 0 ) return;
        m_samplers[idx]->play();
    }
}

void MultiSampler::playRandom()
{
    float rand = (float) std::rand()/RAND_MAX;
    quint16 srand = m_samplers.size()*rand;

    play(srand);
}

void MultiSampler::stop(QVariant variant)
{
    if ( variant.type() == QMetaType::Int )
        m_samplers[variant.toInt()]->stop();

    else if ( variant.type() == QMetaType::QString )
    {
        auto idx = m_files.indexOf(variant.toString());
        if ( idx < 0 ) return;
        m_samplers[idx]->stop();
    }
}


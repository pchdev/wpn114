#include "sampler.hpp"

Sampler::Sampler() : m_loop(false), m_xfade(0), m_attack(0), m_release(0),
    m_start(0), m_length(0), m_rate(1.0), m_stream(false)
{

}

void Sampler::componentComplete()
{
    if ( m_path.isEmpty() ) return;

    m_sfile = new Soundfile(m_path);

    quint64 nch = m_sfile->nchannels();
    m_sfile->setBufSize(m_length*nch);

    SETN_OUT ( nch );
}


void Sampler::userInitialize(qint64)
{
    if ( m_stream ) m_buffer = m_sfile->stream();
    else m_buffer = m_sfile->buffer(m_start);
}

float** Sampler::userProcess(float**, qint64 le)
{


}

void Sampler::setStream(bool stream)
{
    m_stream = stream;
}

void Sampler::setPath(QString path)
{
    m_path = path;
}

void Sampler::setLoop(bool loop)
{
    m_loop = loop;
}

void Sampler::setXfade(quint32 xfade)
{
    m_xfade = xfade;
}

void Sampler::setAttack(quint32 attack)
{
    m_attack = attack;
}

void Sampler::setRelease(quint32 release)
{
    m_release = release;
}

void Sampler::setStart(qreal start)
{
    m_start = start;
}

void Sampler::setLength(qreal length)
{
    m_length = length;
}

void Sampler::setRate(qreal rate)
{
    m_rate = rate;
}

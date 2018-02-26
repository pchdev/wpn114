#include "sharpen.hpp"

Sharpen::Sharpen() : m_depth(0) {}

ON_COMPONENT_COMPLETED( Sharpen )
{
    INITIALIZE_AUDIO_IO;
    //std::calloc( m_ssd, sizeof(qreal)*m_num_inputs );
}

float** Sharpen::process(const quint16 nsamples)
{

    qreal depth = m_depth*2/(1-qMin(m_depth, 0.999));





}

void Sharpen::setDepth(const qreal) {}
qreal Sharpen::depth() const {}


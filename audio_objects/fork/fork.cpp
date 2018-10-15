#include "fork.hpp"

Fork::Fork() : StreamNode()
{

}

void Fork::setTarget(StreamNode* target)
{
    m_target = target;
    target->appendSubnode(this);

    auto parent = qobject_cast<StreamNode*>(QObject::parent());

    SETN_IN  ( parent->numOutputs() );
    SETN_OUT ( parent->numOutputs() );
}

void Fork::initialize(qint64 nsamples)
{

}

float** Fork::process(float** buf, qint64 nsamples)
{
    StreamNode::mergeBuffers(m_out, buf, m_num_outputs, m_num_inputs, nsamples);
    StreamNode::applyGain(m_out, m_num_outputs, nsamples, m_level);

    return m_out;
}

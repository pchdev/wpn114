#include "fork.hpp"
#include <QtDebug>

ForkEndpoint::ForkEndpoint(Fork& fork) : StreamNode(), m_fork(fork)
{
    SETN_IN     ( 0 );
    SETN_OUT    ( fork.numOutputs() );
}

float** ForkEndpoint::process(float** buf, qint64 nsamples)
{
    return m_fork.process(buf, nsamples);
}

Fork::Fork() : StreamNode(), m_parent(nullptr), m_target(nullptr), m_endpoint(nullptr)
{
    m_active = false;
}

void Fork::setTarget(StreamNode* target)
{
    m_target = target;
    m_parent = qobject_cast<StreamNode*>(QObject::parent());

    m_endpoint = new ForkEndpoint(*this);
    m_target->appendSubnode(m_endpoint);
}

void Fork::setActive(bool active)
{
    // this is to keep parent from processing the fork
    // only the receiver gets to process it
    if ( m_endpoint ) m_endpoint->setActive(active);
}

void Fork::preinitialize(StreamProperties properties)
{
    SETN_IN  ( m_parent->numOutputs() );
    SETN_OUT ( m_parent->numOutputs() );

    if ( m_endpoint ) m_endpoint->setNumOutputs(m_num_outputs);

    // parent's num outputs may not always be initialized on time
    // so we have to set it up here, and initialize output buffer
    // (no need to initialize input buffer, as it is copied from parent's output)
    m_stream_properties = properties;
    StreamNode::allocateBuffer(m_out, m_num_outputs, properties.block_size);
}

float** Fork::process(float**, qint64 nsamples)
{
    auto in     = m_parent->outputBuffer();
    auto nout   = m_num_outputs;
    auto out    = m_out;

    StreamNode::resetBuffer(out, m_num_outputs, nsamples);
    StreamNode::mergeBuffers(out, in, m_num_outputs, m_num_outputs, nsamples);
    StreamNode::applyGain(out, m_num_outputs, nsamples, m_level);

    return out;
}

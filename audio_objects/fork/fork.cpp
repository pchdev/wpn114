#include "fork.hpp"
#include <QtDebug>

ForkEndpoint::ForkEndpoint(Fork& fork) : StreamNode(), m_fork(fork)
{
    SETN_IN     ( 0 );
    SETN_OUT    ( fork.numOutputs() );
}

float** ForkEndpoint::process(float** buf, qint64 nsamples)
{
    return m_fork.preprocess(buf, nsamples);
}

Fork::Fork() : StreamNode(), m_parent(nullptr), m_target(nullptr), m_endpoint(nullptr)
{
}

void Fork::setTarget(StreamNode* target)
{
    m_target = target;
    m_parent = qobject_cast<StreamNode*>(QObject::parent());

    m_endpoint = new ForkEndpoint(*this);
    m_target->appendSubnode(m_endpoint);   
}

void Fork::setPrefader(bool prefader)
{
    m_prefader = prefader;
}

void Fork::setActive(bool active)
{
    // this is to keep parent from processing the fork
    // only the receiver gets to process it
    if ( m_endpoint ) m_endpoint->setActive(active);
}

void Fork::onSourceActiveChanged()
{
    auto parent_active = m_parent->active();

    // if parent goes inactive, fork should be inactive too (and the endpoint)
    // if parent goes active again, this should go active ONLY IF
    // 'active' property has not been explicitely set to 'false' at startup

    if ( ! parent_active ) setActive(false);
    else if ( parent_active && m_active_default ) setActive(true);
}

void Fork::preinitialize(StreamProperties properties)
{
    SETN_IN  ( m_parent->numOutputs() );
    SETN_OUT ( m_parent->numOutputs() );

    if ( m_endpoint ) m_endpoint->setNumOutputs(m_num_outputs);
    m_active_default = m_active;
    m_active = false;

    QObject::connect(m_parent, SIGNAL(activeChanged()), this, SLOT(onSourceActiveChanged()));

    // parent's num outputs may not always be initialized on time
    // so we have to set it up here, and initialize output buffer
    // (no need to initialize input buffer, as it is copied from parent's output)
    m_stream_properties = properties;
    StreamNode::allocateBuffer(m_out, m_num_outputs, properties.block_size);
}

float** Fork::preprocess(float**, qint64 nsamples)
{
    auto in     = m_parent->outputBuffer();
    auto nout   = m_num_outputs;
    auto out    = m_out;

    StreamNode::resetBuffer(out, nout, nsamples);
    StreamNode::mergeBuffers(out, in, nout, nout, nsamples);

    if ( m_prefader )
    {
        // a bit of a structural problem here,
        // because target's gain has already been applied
        // so we have to compensate to get it back to normal
        // this allows target not to make a copy of its output buffer and then apply gain
        // but then again, there's no telling when the fork is pulling the target's buffer
        // so it is still problematic when target's level changes
        if ( m_parent->level() > 0.f )
        {
            float temp = 1.f/m_parent->level();
            StreamNode::applyGain(out, nout, nsamples, m_level*temp);
        }
    }

    else StreamNode::applyGain(out, nout, nsamples, m_level);

    return out;
}

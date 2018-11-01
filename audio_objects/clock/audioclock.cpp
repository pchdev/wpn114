#include "audioclock.hpp"
#include <QtDebug>
#include <QThread>
#include <iostream>

TimeNode::TimeNode()
{
    QObject::connect( this, &TimeNode::start, this, &TimeNode::onBegin );
    QObject::connect( this, &TimeNode::end, this, &TimeNode::onStop );
}

TimeNode::TimeNode(const TimeNode& copy)
{
    setDate         ( copy.m_date );
    setDuration     ( copy.m_duration );
    setCondition    ( copy.m_condition );
    setSource       ( copy.m_source );

    QObject::connect( this, &TimeNode::start, this, &TimeNode::onBegin );
    QObject::connect( this, &TimeNode::end, this, &TimeNode::onStop );
}

qreal TimeNode::duration() const
{
    return m_duration;
}

qreal TimeNode::date() const
{
    return m_date;
}

void TimeNode::setDate(qreal date)
{
    m_date = date;
}


void TimeNode::setExposeDevice(WPNDevice* device)
{
    m_expose_device = device;
}

void TimeNode::setExposePath(QString path)
{
    m_expose_path = path;

    if ( !m_expose_device ) m_expose_device = WPNDevice::instance();
    if ( !m_expose_device ) return;

    auto root   = m_expose_device->findOrCreateNode(path);
    auto start  = root->createSubnode("start");
    auto end    = root->createSubnode("end");

    start->setType  ( Type::Impulse );
    end->setType    ( Type::Impulse );

    QObject::connect( start, SIGNAL(valueReceived(QVariant)), this, SIGNAL(start()));
    QObject::connect( end, SIGNAL(valueReceived(QVariant)), this, SIGNAL(end()));
}

void TimeNode::setDuration(qreal duration)
{
    m_duration = duration;
    if ( duration < 0 ) m_infinite = true;
}

void TimeNode::setCondition(bool condition)
{
    m_condition = condition;
}

void TimeNode::setSource(WorldStream *source)
{
    if ( source != m_source )
    {
        m_source = source;
        for ( const auto& subnode : m_subnodes )
             subnode->setSource(source);

        emit sourceChanged();
    }
}

void TimeNode::setFollow(TimeNode* follow)
{
    if ( follow != m_follow )
    {
        m_follow = follow;
        emit afterChanged();

        QObject::connect( follow, &TimeNode::end, this, &TimeNode::onBegin );
        QObject::disconnect( this, &TimeNode::start, this, &TimeNode::onBegin );
    }
}

void TimeNode::setParentNode(TimeNode* parent)
{
    if ( m_parent_node != parent)
    {
        m_parent_node = parent;
        emit parentNodeChanged();
    }
}

void TimeNode::setStartExpression(bool expr)
{
    m_has_start_expression = true;
    if ( m_running && expr ) start();
}

void TimeNode::setEndExpression(bool expr)
{
    m_has_end_expression = true;
    if ( m_running && expr ) end();
}

void TimeNode::setRunning(bool run)
{
    m_running = run;
}

void TimeNode::componentComplete()
{
    if ( !m_parent_node )
         setParentNode(qobject_cast<TimeNode*>(QObject::parent()));

    if ( !m_source && m_parent_node )
         setSource(m_parent_node->source());
}

void TimeNode::onTick(qint64 sz)
{
    if ( m_suspended ) return;

    // if following
    // wait for date to continue
    if ( !m_running && m_follow &&
         m_date >= m_clock &&
         m_date < m_clock+sz )
    {
        m_clock     = 0;
        m_running   = true;

        emit start();
    }

    else if ( !m_running && m_follow )
    {
        m_clock += sz;
        return;
    }

    for ( const auto& subnode : m_subnodes )
    {
        auto date = subnode->date();

        if ( subnode->hasStartExpression() )
             subnode->setRunning(true);

        else if ( date >= m_clock &&
             date < m_clock+sz &&
             subnode->condition() &&
             !subnode->follow())
        {
            subnode->start();
        }
    }

    if ( !m_infinite &&
         m_duration >= m_clock &&
         m_duration < m_clock+sz )
    {
        emit end();
        return;
    }

    m_clock += sz;
}

void TimeNode::onBegin()
{
    if ( !m_follow ) m_running = true;
    QObject::connect(m_source, &WorldStream::tick, this, &TimeNode::onTick);
}

void TimeNode::onStop()
{
    m_clock     = 0;
    m_running   = false;

    QObject::disconnect(m_source, &WorldStream::tick, this, &TimeNode::onTick);
}

void TimeNode::reset()
{
    m_clock = 0;
}

void TimeNode::suspend()
{
    m_suspended = true;
}

void TimeNode::resume()
{
    m_suspended = false;
}

void TimeNode::playFrom(quint64 date)
{
    m_clock = date;
    start();
}

quint64 TimeNode::absoluteDate() const
{
    TimeNode* parent = qobject_cast<TimeNode*>(QObject::parent());
    quint64 absolute = m_date;

    while ( parent ) absolute += parent->date();
    return absolute;
}

QQmlListProperty<TimeNode> TimeNode::subnodes()
{
    return QQmlListProperty<TimeNode>( this, this,
                           &TimeNode::appendSubnode,
                           &TimeNode::subnodesCount,
                           &TimeNode::subnode,
                           &TimeNode::clearSubnodes );
}

TimeNode* TimeNode::subnode(int index) const
{
    return m_subnodes.at(index);
}

void TimeNode::appendSubnode(TimeNode* subnode)
{
    m_subnodes.append(subnode);
}

int TimeNode::subnodesCount() const
{
    return m_subnodes.count();
}

void TimeNode::clearSubnodes()
{
    m_subnodes.clear();
}

// statics --

void TimeNode::appendSubnode(QQmlListProperty<TimeNode>* list, TimeNode* subnode)
{
    reinterpret_cast<TimeNode*>(list->data)->appendSubnode(subnode);
}

void TimeNode::clearSubnodes(QQmlListProperty<TimeNode>* list )
{
    reinterpret_cast<TimeNode*>(list->data)->clearSubnodes();
}

TimeNode* TimeNode::subnode(QQmlListProperty<TimeNode>* list, int i)
{
    return reinterpret_cast<TimeNode*>(list->data)->subnode(i);
}

int TimeNode::subnodesCount(QQmlListProperty<TimeNode>* list)
{
    return reinterpret_cast<TimeNode*>(list->data)->subnodesCount();
}

// LOOP -----------------------------------------------------------------------------------------

Loop::Loop() : TimeNode(), m_pattern(new TimeNode)
{
    QObject::connect( this, &TimeNode::sourceChanged, this, &Loop::onSourceChanged );
}

void Loop::setTimes(quint64 times)
{
    m_times = times;
}

void Loop::setPattern(TimeNode* pattern)
{
    // nothing to do here for now
}

void Loop::onSourceChanged()
{
    m_pattern->setSource(m_source);
}

void Loop::componentComplete()
{
    TimeNode::componentComplete();
    m_pattern->componentComplete();

    if ( m_times ) m_duration = m_pattern->duration()*m_times;

    QObject::connect( this, &Loop::start, m_pattern, &TimeNode::start );
    QObject::connect( this, &Loop::end, m_pattern, &TimeNode::end );
    QObject::connect( m_pattern, &TimeNode::end, this, &Loop::onPatternStop );
}

void Loop::onPatternStop()
{
    if ( m_running ) m_pattern->start();
}

void Loop::onTick(qint64 sz)
{
    TimeNode::onTick(sz);
    //m_pattern->onTick(sz);
}

TimeNode* Loop::subnode(int index) const
{
    return m_pattern->subnode(index);
}

void Loop::appendSubnode(TimeNode* subnode)
{
    return m_pattern->appendSubnode(subnode);
}

int Loop::subnodesCount() const
{
    return m_pattern->subnodesCount();
}

void Loop::clearSubnodes()
{
    m_pattern->clearSubnodes();
}

// AUTOMATION -----------------------------------------------------------------------------------------

Automation::Automation() : TimeNode(), m_target(nullptr)
{

}

void Automation::setFrom(qreal from)
{
    m_from = from;
}

void Automation::setTo(qreal to)
{
    m_to = to;
}

void Automation::setTarget(QObject* target)
{
    m_target = target;
}

void Automation::setProperty(QString property)
{
    m_property_str = property;
}

void Automation::componentComplete()
{
    TimeNode::componentComplete();

    if ( !m_target ) return;

    int idx = m_target->metaObject()->indexOfProperty(
                m_property_str.toStdString().c_str() );

    m_property = m_target->metaObject()->property(idx);
}

void Automation::onBegin()
{
    TimeNode::onBegin();

    // this is to avoid qml's property bindings
    // the values will be evaluated on startup

    m_ex_from   = m_from;
    m_ex_to     = m_to;

    if ( !m_follow && m_property.isWritable())
         m_property.write(m_target, m_ex_from);

    else if ( m_follow )
        QObject::connect( this, &TimeNode::start, this, &Automation::onFollowBegin );
}

void Automation::onFollowBegin()
{
    if ( m_property.isWritable())
         m_property.write(m_target, m_ex_from);
}

void Automation::onStop()
{
    TimeNode::onStop();

    if ( m_property.isWritable())
         m_property.write(m_target, m_ex_to);

    if ( m_follow )
        QObject::disconnect( this, &TimeNode::start, this, &Automation::onFollowBegin );
}

void Automation::onTick(qint64 sz)
{
    TimeNode::onTick(sz);

    if ( !m_running ) return;

    m_phase = m_clock/m_duration;

    if ( m_property.isWritable())
         m_property.write(m_target, m_ex_from+(m_ex_to-m_ex_from)*m_phase);
}

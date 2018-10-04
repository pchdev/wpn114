#include "rooms.hpp"
#include <cmath>

SpeakerRing::SpeakerRing()
{

}

void SpeakerRing::setOffset(qint16 offset)
{
    m_offset = offset;
}

void SpeakerRing::setElevation(qreal elevation)
{
    m_elevation = elevation;
}

void SpeakerRing::setHeight(qreal height)
{
    m_height = height;
}

void SpeakerRing::setWidth(qreal width)
{
    m_width = width;
}

void SpeakerRing::componentComplete()
{
    // TODO: elliptic form with width&height
    for ( quint16 ls = 0; ls < m_nspeakers; ++ls )
    {
        QVector3D position;
        qreal x = (sin((qreal)ls/m_nspeakers*M_PI*2 + m_offset) + 1.f) / 2.f;
        qreal y = (cos((qreal)ls/m_nspeakers*M_PI*2 + m_offset) + 1.f) / 2.f;

        position.setX ( x );
        position.setY ( y );
        position.setZ ( m_elevation );
    }
}

//---------------------------------------------------------------------------------------------------------

RoomNode::RoomNode() : m_nspeakers(0)
{

}

void RoomNode::setNspeakers(quint16 nspeakers)
{
    m_nspeakers = nspeakers;
}

void RoomNode::setInfluence(QVariant influence)
{
    m_influence = influence;
    m_influences.clear();

    if ( influence.type() == QMetaType::QVariantList )
        for ( const auto& i : influence.toList() )
            m_influences << i.toDouble();

    else if ( influence.type() == QMetaType::Double ||
              influence.type() == QMetaType::Float )
    {
        if ( m_nspeakers > 1 )
             m_influences.fill(influence.toDouble(), m_nspeakers);
        else m_influences << influence.toDouble();
    }
}

void RoomNode::setPosition(QVariant position)
{
    m_position = position;
    m_positions.clear();

    if ( position.type() == QMetaType::QVariantList )
    {
        QVector3D pos;
        for ( const auto& p : position.toList())
        {
            auto l = p.toList();
            if ( l.size() == 2 )
            {
                pos.setX(l[0].toDouble());
                pos.setY(l[1].toDouble());
                pos.setZ(0.f);
            }
            else if ( l.size() == 3 )
            {
                pos.setX(l[0].toDouble());
                pos.setY(l[1].toDouble());
                pos.setZ(l[2].toDouble());
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------

RoomSetup::RoomSetup() : m_nspeakers(0)
{

}

void RoomSetup::componentComplete()
{
    for ( const auto& node : m_nodes )
        m_nspeakers += node->nspeakers();
}

QQmlListProperty<RoomNode> RoomSetup::nodes()
{
    return QQmlListProperty<RoomNode>( this, this,
                           &RoomSetup::appendNode,
                           &RoomSetup::nodeCount,
                           &RoomSetup::node,
                           &RoomSetup::clearNodes );
}

void RoomSetup::appendNode(RoomNode* node)
{
    m_nodes.append(node);
}

int RoomSetup::nodeCount() const
{
    return m_nodes.count();
}

RoomNode* RoomSetup::node(int index) const
{
    return m_nodes.at(index);
}

void RoomSetup::clearNodes()
{
    m_nodes.clear();
}

void RoomSetup::appendNode(QQmlListProperty<RoomNode>* list, RoomNode* node)
{
    reinterpret_cast<RoomSetup*>(list->data)->appendNode(node);
}

int RoomSetup::nodeCount(QQmlListProperty<RoomNode>* list)
{
    return reinterpret_cast<RoomSetup*>(list->data)->nodeCount();
}

RoomNode* RoomSetup::node(QQmlListProperty<RoomNode>* list, int index)
{
    return reinterpret_cast<RoomSetup*>(list->data)->node(index);
}

void RoomSetup::clearNodes(QQmlListProperty<RoomNode>* list)
{
    reinterpret_cast<RoomSetup*>(list->data)->clearNodes();
}

//---------------------------------------------------------------------------------------------------------

RoomSource::RoomSource()
{

}

void RoomSource::setBias(QVariant bias)
{
    m_bias = bias;
    update();
}

void RoomSource::setDiffuse(QVariant diffuse)
{
    m_diffuse = diffuse;
    update();
}

void RoomSource::setPosition(QVariant position)
{
    m_position = position;
    update();
}

void RoomSource::setRotate(QVariant rotate)
{
    m_rotate = rotate;
    update();
}

void RoomSource::update()
{

}

void RoomSource::userInitialize(qint64 nsamples)
{

}

float** RoomSource::userProcess(float**buf, qint64 nsamples)
{

}

//---------------------------------------------------------------------------------------------------------

Rooms::Rooms()
{

}

void Rooms::setSetup(RoomSetup* setup)
{
    m_setup = setup;
}

void Rooms::userInitialize(qint64 nsamples)
{

}

float** Rooms::process(float** buf, qint64 nsamples)
{
    for ( const auto& node : m_subnodes )
    {
        auto source = qobject_cast<RoomSource*>(node);
        if ( ! source ) continue;

        //...

    }

}

// no need to call userProcess, as process is already overriden
float** Rooms::userProcess(float** buf, qint64 nsamples) { }

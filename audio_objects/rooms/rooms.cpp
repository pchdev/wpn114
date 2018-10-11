#include "rooms.hpp"
#include <cmath>
#include <QtDebug>

SpeakerRing::SpeakerRing()
{

}

void SpeakerRing::setOffset(qreal offset)
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
    QVariantList list;

    for ( quint16 ls = 0; ls < m_nspeakers; ++ls )
    {
        QVector3D position;
        qreal x = (sin((qreal)ls/m_nspeakers*M_PI*2 + m_offset) + 1.f) / 2.f;
        qreal y = (cos((qreal)ls/m_nspeakers*M_PI*2 + m_offset) + 1.f) / 2.f;

        position.setX ( x );
        position.setY ( y );
        position.setZ ( m_elevation );

        list << position;
        m_positions << position;
    }

    m_position = list;
}

//---------------------------------------------------------------------------------------------------------

RoomNode::RoomNode() : m_nspeakers(0)
{

}

void RoomNode::setNspeakers(quint16 nspeakers)
{
    m_nspeakers = nspeakers;
    setInfluence(m_influence);
}

QVector4D RoomNode::speakerData(quint16 index) const
{
    if ( !m_positions.size() ) return QVector4D();
    QVector4D res(m_positions[index], m_influences[index]);
    return res;
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

const QVector<QVector4D> RoomSetup::speakers() const
{
    QVector<QVector4D> res;

    for ( const auto& node: m_nodes )
        for ( quint16 i = 0; i < node->nspeakers(); ++i )
            res << node->speakerData(i);

    return res;
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

RoomSource::RoomSource() : m_position(QVector3D(0.5, 0.5, 0.5))
{
    SETN_IN ( 0 );
}

void RoomSource::componentComplete()
{
    SETN_OUT  ( m_max_outputs );
}

void RoomSource::update()
{
    if ( m_diffuse > 0 )
    {
        qreal ellwidth  = m_diffuse*(m_bias/0.5)/2.0;
        qreal ellheight = 1.0/ellwidth/2.0;

        // rotate todo
        m_extremities[0] = QVector3D(m_position.x(), m_position.y()+ellheight, m_position.z());
        m_extremities[1] = QVector3D(m_position.x(), m_position.y()-ellheight, m_position.z());
        m_extremities[2] = QVector3D(m_position.x()-ellwidth, m_position.y(), m_position.z());
        m_extremities[3] = QVector3D(m_position.x()+ellwidth, m_position.y(), m_position.z());
    }
}

void RoomSource::setPosition(QVector3D position)
{
    m_position = position;
    update();
}

void RoomSource::setDiffuse(qreal diffuse)
{
    m_diffuse = diffuse;
    update();
}

void RoomSource::setBias(qreal bias)
{
    m_bias = bias;
    update();
}

void RoomSource::setRotate(qreal rotate)
{
    m_rotate = rotate;
    update();
}

void RoomSource::setX(qreal x)
{
    m_position.setX(x);
}

void RoomSource::setY(qreal y)
{
    m_position.setY(y);
}

void RoomSource::setFixed(bool fixed)
{
    m_fixed = fixed;
}

void RoomSource::userInitialize(qint64 nsamples)
{

}

float** RoomSource::userProcess(float** buf, qint64 nsamples)
{
    auto out = m_out;
    auto nout = m_num_outputs;

    for ( const auto& node : m_subnodes )
        StreamNode::mergeBuffers( out, node->userProcess(buf, nsamples),
                                  nout, node->numOutputs(), nsamples );

    return out;
}

// STEREO

RoomStereoSource::RoomStereoSource() :
    m_left(new RoomSource),
    m_right(new RoomSource)
{

}

void RoomStereoSource::setXspread(qreal xspread)
{
    m_xspread = xspread;

    auto lx = m_left->position();
    auto rx = m_right->position();

    lx.setX(0.5-xspread);
    rx.setX(0.5+xspread);

    m_left  ->setPosition(lx);
    m_right ->setPosition(rx);
}

void RoomStereoSource::setYspread(qreal yspread)
{
    m_yspread = yspread;

    auto ly = m_left->position();
    auto ry = m_right->position();

    ly.setY(0.5-yspread);
    ry.setY(0.5+yspread);

    m_left  ->setPosition(ly);
    m_right ->setPosition(ry);
}

void RoomStereoSource::setDiffuse(qreal diffuse)
{
    m_diffuse = diffuse;

    m_left  ->setDiffuse(diffuse);
    m_right ->setDiffuse(diffuse);
}

void RoomStereoSource::setRotate(qreal rotate)
{
    m_rotate = rotate;

    m_left  ->setRotate(rotate);
    m_right ->setRotate(rotate);
}

void RoomStereoSource::setBias(qreal bias)
{
    m_bias = bias;

    m_left  ->setBias(bias);
    m_right ->setBias(bias);
}

void RoomStereoSource::setX(qreal x)
{
    m_x = x;

    m_left  ->setX(x);
    m_right ->setX(x);
}

void RoomStereoSource::setY(qreal y)
{
    m_y = y;

    m_left  ->setY(y);
    m_right ->setY(y);
}

//---------------------------------------------------------------------------------------------------------

Rooms::Rooms() {}

void Rooms::setSetup(RoomSetup* setup)
{
    m_setup     = setup;
    m_speakers  = setup->speakers();

    SETN_OUT ( setup->nspeakers() );
}

qreal Rooms::spgain(QVector3D src, QVector4D ls)
{
    // only in 2D for now
    auto lr  = ls.w();
    auto s   = src.toVector2D();
    auto l   = ls.toVector2D();

    float dx = fabs(s.x()-l.x());
    if ( dx > lr ) return 0;

    float dy = fabs(s.y()-l.y());
    if ( dy > lr ) return 0;

    float d = sqrt((dx*dx)+(dy*dy));
    if ( d/lr > 1 ) return 0;
    else return (1.f - d/lr);
}

void Rooms::computeCoeffs(RoomSource& source)
{

}

void Rooms::userInitialize(qint64 nsamples)
{        
    for ( const auto& node : m_subnodes )
    {
        auto source = qobject_cast<RoomSource*>(node);
        if ( ! source ) continue;

        // manage coefficents for static sources
        if ( source->fixed() ) computeCoeffs(*source);
    }
}

float** Rooms::process(float** buf, qint64 nsamples)
{    
    auto speakers   = m_speakers;
    auto out        = m_out;
    auto nout       = m_num_outputs;

    StreamNode::resetBuffer(out, nout, nsamples);

    for ( const auto& node : m_subnodes )
    {
        auto source = qobject_cast<RoomSource*>(node);
        if ( !source ) continue;

        quint16 snch = source->numOutputs();
        float** in  = source->process(buf, nsamples);

        // if source's not supposed to move, dont re-calculate the coeffs
        if ( !source->fixed() ) computeCoeffs(*source);

        /*auto stc = source->coeffs();

        for ( quint16 ch = 0; ch < snch; ++ch )
            for ( quint16 o = 0; o < nout; ++o )
                for (quint16 s = 0; s < nsamples; ++s)
                    out[o][s] += in[ch][s] * stc[ch][o];*/
    }

    return out;
}

// no need to call userProcess, as process is already overriden
float** Rooms::userProcess(float** buf, qint64 nsamples) { }

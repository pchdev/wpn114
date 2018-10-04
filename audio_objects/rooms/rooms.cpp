#include "rooms.hpp"
#include <cmath>

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
}

QVector4D RoomNode::speakerData(quint16 index) const
{
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

RoomSource::RoomSource()
{
    SETN_IN ( 0 );
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

void RoomSource::setFixed(bool fixed)
{
    m_fixed = fixed;
}

void RoomSource::update()
{

}

void RoomSource::allocateCoeffVector(quint16 size)
{
    m_coeffs.reserve(m_nchannels);

    for ( auto& ch : m_coeffs )
        ch.reserve(size);
}

void RoomSource::setCoeffs(QVector<QVector<qreal>> coeffs)
{
    m_coeffs = coeffs;
}

void RoomSource::userInitialize(qint64 nsamples)
{
    update();
}

float** RoomSource::userProcess(float**buf, qint64 nsamples)
{
    return m_subnodes[0]->process(buf, nsamples);
}

//---------------------------------------------------------------------------------------------------------

Rooms::Rooms()
{

}

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
    auto schs   = source.channels();
    auto coeffs = source.coeffs();
    quint16 ch  = 0;

    for ( const auto& sch : schs )
    {
        auto channel_coeffs = coeffs[ch];
        for ( quint16 sp = 0; sp < m_speakers.size(); ++sp)
        {
            qreal maxgain = 0;
            for ( const auto& mirror : sch )
            {
                // for each source's point of diffusion
                // we choose the one with the maximum gain
                qreal g = spgain(mirror, m_speakers[sp]);
                if ( g > maxgain ) maxgain = g;
            }
            channel_coeffs[sp] = maxgain;
        }
        ++ch;
    }
}

void Rooms::userInitialize(qint64 nsamples)
{
    for ( const auto& node : m_subnodes )
    {
        auto source = qobject_cast<RoomSource*>(node);
        if ( ! source ) continue;

        source->allocateCoeffVector(m_num_outputs);

        // manage coefficents for static sources
        if ( source->fixed() ) computeCoeffs(*source);
    }
}

float** Rooms::process(float** buf, qint64 nsamples)
{    
    auto speakers   = m_speakers;
    auto out        = m_out;
    auto nout       = m_num_outputs;

    for ( const auto& node : m_subnodes )
    {
        auto source = qobject_cast<RoomSource*>(node);
        if ( ! source ) continue;

        quint16 nch = source->numOutputs();
        float** in  = source->process(buf, nsamples);

        if ( !source->fixed() ) computeCoeffs(*source);

        auto stc = source->coeffs();

        for ( quint16 ch = 0; ch < nch; ++ch )
            for ( quint16 o = 0; o < nout; ++o )
                for (quint16 s = 0; s < nsamples; ++s)
                    out[o][s] += in[ch][s];
    }

    return out;
}

// no need to call userProcess, as process is already overriden
float** Rooms::userProcess(float** buf, qint64 nsamples) { }

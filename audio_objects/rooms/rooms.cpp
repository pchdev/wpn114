#include "rooms.hpp"
#include <cmath>
#include <QtDebug>

SpeakerPair::SpeakerPair()
{
    setNspeakers(2);
}

void SpeakerPair::setXspread(qreal xspread)
{
    m_xspread = xspread;
}

void SpeakerPair::setYspread(qreal yspread)
{
    m_yspread = yspread;
}

void SpeakerPair::setX(qreal x)
{
    m_x = x;
}

void SpeakerPair::setY(qreal y)
{
    m_y = y;
}

void SpeakerPair::componentComplete()
{
    QVector3D position_l, position_r;
    QVariantList list;

    position_l.setX(0.5-m_xspread);
    position_r.setX(0.5+m_xspread);
    position_l.setY(m_y);
    position_r.setY(m_y);

    list << position_l << position_r;
    m_positions << position_l << position_r;

    m_position = list;
}

//--------------------------------------------------------------------------------------------------

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

RoomSetup::~RoomSetup()
{
    for ( const auto& node : m_nodes )
        delete node;
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
    SETN_IN   ( 0 );
}

void RoomSource::componentComplete()
{
    SETN_OUT  ( nchannels() );
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
    m_x = x;
}

void RoomSource::setY(qreal y)
{
    m_y = y;
}

void RoomSource::setFixed(bool fixed)
{
    m_fixed = fixed;
}

void RoomSource::initialize(qint64 nsamples)
{
    if ( m_subnodes.size() != 1 ) return;

    // if single source, link 'active' property
    onSingleSourceActiveChanged();
    QObject::connect(m_subnodes[0], SIGNAL(activeChanged()), this, SLOT(onSingleSourceActiveChanged()));
}

void RoomSource::onSingleSourceActiveChanged()
{
    m_active = m_subnodes[0]->active();
}

float** RoomSource::preprocess(float** buf, qint64 nsamples)
{
    if ( m_subnodes.size() == 1 && m_subnodes[0]->active())
        return m_subnodes[0]->preprocess(buf, nsamples);

    auto out = m_out;
    auto nout = m_num_outputs;
    StreamNode::resetBuffer(out, nout, nsamples);

    for ( const auto& node : m_subnodes )
    {
        if ( !node->active() ) continue;
        StreamNode::mergeBuffers( out, node->preprocess(buf, nsamples),
                                  nout, node->numOutputs(), nsamples );
    }

    return out;
}

// CHANNEL ---------------------------------------------------------------------------------------

qreal RoomChannel::spgain(QVector3D src, QVector4D ls)
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

void RoomChannel::computeCoeffs()
{
    quint16 spk = 0;
    for ( const auto& speaker: speakers )
    {
        qreal gain = 0.0;

        if ( !diffuse ) gain = spgain(c, speaker);
        else
        {
            qreal cg = spgain(c, speaker);
            qreal ng = spgain(n, speaker);
            qreal sg = spgain(s, speaker);
            qreal eg = spgain(e, speaker);
            qreal wg = spgain(w, speaker);

            gain = qMax(qMax(qMax(qMax(ng,sg),eg),wg),cg);

            //qDebug() << "Gain for speaker:" << spk << gain;
        }

        coeffs[spk] = gain;
        spk++;
    }
}

// MONO ------------------------------------------------------------------------------------------

MonoSource::MonoSource() : RoomSource()
{
    m_channel.c = QVector3D(0.5, 0.5, 0.5);
}

void MonoSource::allocateCoeffs(QVector<QVector4D> const& speakerset)
{
    auto coeffs = m_channel.coeffs;
    if ( coeffs ) delete coeffs;

    coeffs = new float[speakerset.size()]();

    m_channel.coeffs = coeffs;
    m_channel.speakers = speakerset;
}

RoomChannel& MonoSource::channel(quint16)
{
    return m_channel;
}

void MonoSource::componentComplete()
{
    RoomSource::componentComplete();

    if       ( m_bias == 0.5 )  { m_w = m_diffuse; m_h = m_diffuse; }
    else if  ( m_bias < 0.5  )  { m_w = m_diffuse; m_h = m_bias; }
    else if  ( m_bias > 0.5  )  { m_h = m_diffuse; m_w = m_bias-0.5; }

    m_w /= 2.0; m_h /= 2.0;
    update();
}

void MonoSource::update()
{
    if ( m_diffuse > 0 )
    {
        m_channel.diffuse = true;

        // rotate todo
        m_channel.n = QVector3D(m_x, m_y+m_h, 0.5);
        m_channel.s = QVector3D(m_x, m_y-m_h, 0.5);
        m_channel.w = QVector3D(m_x-m_w, m_y, 0.5);
        m_channel.e = QVector3D(m_x+m_w, m_y, 0.5);
    }

    else m_channel.diffuse = false;
}

void MonoSource::setPosition(QVector3D position)
{
    m_channel.c = position;
    m_x = position.x();
    m_y = position.y();
    update();
}

void MonoSource::setX(qreal x)
{
    m_x = x;
    m_channel.c.setX(x);
    update();
}

void MonoSource::setY(qreal y)
{
    m_y = y;
    m_channel.c.setY(y);
    update();
}

// STEREO --------------------------------------------------------------------------------------

StereoSource::StereoSource() : RoomSource(),
    m_left  ( new MonoSource ),
    m_right ( new MonoSource )
{

}

StereoSource::~StereoSource()
{
    delete m_left;
    delete m_right;
}

void StereoSource::componentComplete()
{
    RoomSource::componentComplete();
    m_left->componentComplete();
    m_right->componentComplete();
}

void StereoSource::allocateCoeffs(QVector<QVector4D> const& speakerset)
{
    m_left  ->allocateCoeffs ( speakerset );
    m_right ->allocateCoeffs ( speakerset );
}

RoomChannel& StereoSource::channel(quint16 index)
{
    if ( !index ) return m_left->channel(0);
    else return m_right->channel(0);
}

void StereoSource::setXspread(qreal xspread)
{
    m_xspread = xspread;

    m_left  ->setX(0.5-xspread);
    m_right ->setX(0.5+xspread);
}

void StereoSource::setYspread(qreal yspread)
{
    m_yspread = yspread;

    m_left  ->setY(0.5-yspread);
    m_right ->setY(0.5+yspread);
}

void StereoSource::setDiffuse(qreal diffuse)
{
    m_diffuse = diffuse;

    m_left  ->setDiffuse(diffuse);
    m_right ->setDiffuse(diffuse);
}

void StereoSource::setRotate(qreal rotate)
{
    m_rotate = rotate;

    m_left  ->setRotate(rotate);
    m_right ->setRotate(rotate);
}

void StereoSource::setBias(qreal bias)
{
    m_bias = bias;

    m_left  ->setBias(bias);
    m_right ->setBias(bias);
}

void StereoSource::setX(qreal x)
{
    m_x = x;

    m_left  ->setX(x);
    m_right ->setX(x);
}

void StereoSource::setY(qreal y)
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

void Rooms::initialize(qint64 nsamples)
{        
    for ( const auto& node : m_subnodes )
    {
        auto source = qobject_cast<RoomSource*>(node);
        if ( ! source ) continue;

        source->allocateCoeffs(m_speakers);        

        // manage coefficents for static sources
        if ( source->fixed() )
            for ( quint16 ch = 0; ch < source->numOutputs(); ++ch )
                source->channel(ch).computeCoeffs();
    }
}

float** Rooms::preprocess(float** buf, qint64 nsamples)
{    
    auto speakers   = m_speakers;
    auto out        = m_out;
    auto nout       = m_num_outputs;

    StreamNode::resetBuffer(out, nout, nsamples);

    for ( const auto& node : m_subnodes )
    {
        auto source = qobject_cast<RoomSource*>(node);
        if ( !source || !source->active() ) continue;

        quint16 snch = source->numOutputs();
        float** in  = source->preprocess(nullptr, nsamples);

        for ( quint16 ch = 0; ch < snch; ++ch )
        {
            auto channel = source->channel(ch);

            // if source's not supposed to move, dont re-calculate the coeffs
            if ( !source->fixed() ) channel.computeCoeffs();
            auto coeffs = channel.coeffs;

            for ( quint16 o = 0; o < nout; ++o )
                for ( quint16 s = 0; s < nsamples; ++s )
                    out[o][s] += in[ch][s] * coeffs[o];
        }
    }

    return out;
}

#include "rooms.hpp"
#include <cmath>
#include <QtDebug>

SpeakerArea::SpeakerArea()
{

}

SpeakerArea::SpeakerArea(qreal radius, qreal bias, qreal angle) :
    m_radius(radius), m_bias(bias), m_angle(angle)
{

}

SpeakerArea::SpeakerArea(SpeakerArea const& area )
{
    m_radius    = area.radius();
    m_bias      = area.bias();
    m_angle     = area.angle();
}

SpeakerArea& SpeakerArea::operator=( SpeakerArea const& area )
{
    m_radius    = area.radius();
    m_bias      = area.bias();
    m_angle     = area.angle();

    return *this;
}

bool SpeakerArea::operator!=(SpeakerArea const& area)
{
    return ( area.angle() != m_angle &&
             area.radius() != m_radius &&
             area.bias() != m_bias );
}

void SpeakerArea::setRadius(qreal radius)
{
    if ( radius != m_radius )
    {
        m_radius = radius;
        emit radiusChanged();
    }
}

void SpeakerArea::setBias(qreal bias)
{
    if ( bias != m_bias )
    {
        m_bias = bias;
        emit biasChanged();
    }
}

void SpeakerArea::setAngle(qreal angle)
{
    if ( angle != m_angle )
    {
        m_angle = angle;
        emit angleChanged();
    }

}

//----------------------

Speaker::Speaker() : m_position(QVector3D(0.5, 0.5, 0.5))
{

}

Speaker::Speaker(QVector3D position) : m_position(position)
{

}

Speaker::Speaker(Speaker const& copy)
{
    m_position = copy.position();
    m_horizontal_area = copy.horizontalArea();
    m_vertical_area = copy.verticalArea();
}

Speaker& Speaker::operator=(Speaker const& assign)
{
    m_position = assign.position();
    m_horizontal_area = assign.horizontalArea();
    m_vertical_area = assign.verticalArea();
}

void Speaker::setPosition(QVector3D const& position)
{
    if ( position != m_position )
    {
         m_position = position;
         emit positionChanged();
    }
}

void Speaker::setHorizontalArea(SpeakerArea const& area)
{
//    if ( area != m_horizontal_area )
    m_horizontal_area = area;
}

void Speaker::setVerticalArea(SpeakerArea const& area)
{
    m_vertical_area = area;
}

void Speaker::setX(qreal x)
{
    if ( x == m_position.x() ) return;
    m_position.setX(x);
    emit xChanged();
}

void Speaker::setY(qreal y)
{
    if ( y == m_position.y() ) return;
    m_position.setY(y);
    emit yChanged();
}

void Speaker::setZ(qreal z)
{
    if ( z == m_position.z() ) return;
    m_position.setZ(z);
    emit zChanged();
}

SingleSpeaker::SingleSpeaker() : m_speaker(new Speaker)
{
    m_speakers.push_back(m_speaker);
}

//===========================================================================

SpeakerPair::SpeakerPair()
{
    m_left  = new Speaker( QVector3D( 0.25, 0.5, 0.5) );
    m_right = new Speaker( QVector3D( 0.75, 0.5, 0.5) );

    m_speakers.push_back(m_left  );
    m_speakers.push_back(m_right );
}

void SpeakerPair::setXspread(qreal xspread)
{
    if ( xspread == m_xspread ) return;

    m_xspread = xspread;

    m_left->setX    ( 0.5-xspread );
    m_right->setX   ( xspread+0.5 );
}

void SpeakerPair::setYspread(qreal yspread)
{
    if ( yspread == m_yspread ) return;
    m_yspread = yspread;

    m_left->setY    ( 0.5-yspread );
    m_right->setY   ( yspread+0.5 );
}

void SpeakerPair::setZspread(qreal zspread)
{
    if ( zspread == m_zspread ) return;
    m_zspread = zspread;

    m_left->setZ    ( 0.5-zspread );
    m_right->setZ   ( 0.5+zspread );
}

void SpeakerPair::setX(qreal x)
{
    if ( m_x == x ) return;
    m_x = x;

    m_left->setX    ( x );
    m_right->setX   ( x );
}

void SpeakerPair::setY(qreal y)
{
    if ( m_y == y ) return;
    m_y = y;

    m_left->setY    ( y );
    m_right->setY   ( y );
}

void SpeakerPair::setZ(qreal z)
{
    if ( m_z == z ) return;
    m_z = z;

    m_left->setZ    ( z );
    m_right->setZ   ( z );
}

void SpeakerPair::componentComplete()
{
}

//--------------------------------------------------------------------------------------------------

#define CHANGEGUARD(a,b) if (a == b) return
SpeakerRing::SpeakerRing()
{

}

void SpeakerRing::setNspeakers(quint16 nspeakers)
{
    // TODO: update on the fly
    for ( quint16 n = 0; n < nspeakers; ++n )
        m_speakers.push_back(new Speaker());
}

void SpeakerRing::update()
{
    quint16 index = 0;

    for ( const auto& speaker : m_speakers )
    {
        QVector3D position;
        qreal ph = (qreal)index/nspeakers()*M_PI*2 + m_offset;
        qreal x = (sin(ph) +1.)/2.;
        qreal y = (cos(ph) +1.)/2.;

        speaker->setX ( x*m_radius + ((1.-m_radius)/2));
        speaker->setY ( y*m_radius + ((1.-m_radius)/2.));
    }
}

void SpeakerRing::setOffset(qreal offset)
{
    CHANGEGUARD ( offset, m_offset );
    m_offset = offset;
    update();

    emit offsetChanged();
}

void SpeakerRing::setElevation(qreal elevation)
{
    CHANGEGUARD ( elevation, m_elevation );
    m_elevation = elevation;

    for ( const auto& speaker : m_speakers )
        speaker->setZ(elevation);

    emit elevationChanged();
}

void SpeakerRing::setHeight(qreal height)
{
    CHANGEGUARD ( height, m_height );
    m_height = height;

    emit heightChanged();
    // TODO
}

void SpeakerRing::setWidth(qreal width)
{
    CHANGEGUARD ( width, m_width );
    m_width = width;

    emit widthChanged();
    // TODO
}

void SpeakerRing::setRadius(qreal radius)
{
    CHANGEGUARD ( radius, m_radius );
    m_radius = radius;
    update();

    emit radiusChanged();
}

void SpeakerRing::componentComplete()
{

}

//---------------------------------------------------------------------------------------------------------

RoomNode::RoomNode()
{

}

RoomNode::~RoomNode()
{
    for ( const auto& speaker : m_speakers )
          delete speaker;
}

void RoomNode::setHorizontalInfluence(qreal inf)
{
    for ( const auto& speaker : m_speakers )
        speaker->setHorizontalArea(SpeakerArea(inf, 0.5, 0));

    m_h_influence = inf;
}

void RoomNode::setVerticalInfluence(qreal inf)
{
    for ( const auto& speaker : m_speakers )
        speaker->setVerticalArea(SpeakerArea(inf, 0.5, 0));

    m_v_influence = inf;
}

QVariant RoomNode::speakers() const
{
    if ( m_speakers.size() == 0 )
         return QVariant();

    else if ( m_speakers.size() == 1 )
        return QVariant::fromValue(m_speakers[0]);

    else
    {
        QVariantList list;
        for ( const auto& speaker : m_speakers )
            list << QVariant::fromValue( speaker );

        return list;
    }
}

//---------------------------------------------------------------------------------------------------------

RoomSetup::RoomSetup()
{

}

RoomSetup::~RoomSetup()
{
    for ( const auto& node : m_nodes )
          delete node;
}

void RoomSetup::componentComplete()
{

}

QVariantList RoomSetup::speakerList() const
{
    QVariantList list;
    for ( const auto& speaker : m_speakers )
        list << QVariant::fromValue(speaker);

    return list;
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
    m_speakers.append(node->getSpeakers());

    emit nodesChanged();
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
    emit nodesChanged();
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
    SETTYPE   ( StreamType::Effect );
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

void RoomSource::setZ(qreal z)
{
    m_z = z;
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

    QObject::connect( m_subnodes[0], &StreamNode::activeChanged,
            this, &RoomSource::onSingleSourceActiveChanged );
}

void RoomSource::onSingleSourceActiveChanged()
{
    m_active = m_subnodes[0]->active();
}

float** RoomSource::preprocess(float** buf, qint64 nsamples)
{
    if ( m_subnodes.size() == 1 && m_subnodes[0]->active())
        return m_subnodes[0]->preprocess(buf, nsamples);

    auto out  = m_out;
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

qreal RoomChannel::spgain(const QVector3D &src, const Speaker &ls)
{
    auto lrh  = ls.horizontalArea().radius();
    auto lrv  = ls.verticalArea().radius();

    float dx = fabs(src.x()-ls.x());
    if ( dx > lrh ) return 0;

    float dy = fabs(src.y()-ls.y());
    if ( dy > lrh ) return 0;

    float dz = fabs(src.z()-ls.z());
    if ( dz > lrv ) return 0;

    float d = sqrt((dx*dx)+(dy*dy)+(dz*dz));
    if ( d/lrh > 1 ) return 0;
    else return (1.f - d/lrh/lrv);
}

void RoomChannel::computeCoeffs()
{
    quint16 spk = 0;
    for ( const auto& speaker: speakers )
    {
        qreal gain = 0.0;

        if ( !diffuse ) gain = spgain(c, *speaker);
        else
        {
            qreal cg = spgain(c, *speaker);
            qreal ng = spgain(n, *speaker);
            qreal sg = spgain(s, *speaker);
            qreal eg = spgain(e, *speaker);
            qreal wg = spgain(w, *speaker);

            gain = qMax(qMax(qMax(qMax(ng,sg),eg),wg),cg);
        }

        coeffs[spk] = gain;
        spk++;
    }
}

// MONO ------------------------------------------------------------------------------------------

MonoSource::MonoSource() : RoomSource()
{
    m_channel.c = QVector3D( 0.5, 0.5, 0.5 );
}

void MonoSource::allocateCoeffs(QVector<Speaker*> const& speakerset)
{
    auto coeffs = m_channel.coeffs;
    if ( coeffs ) delete coeffs;

    coeffs = new float[ speakerset.size() ]();

    m_channel.coeffs    = coeffs;
    m_channel.speakers  = speakerset;
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

    emit positionChanged();
}

void MonoSource::update()
{
    if ( m_diffuse > 0 )
    {
        m_channel.diffuse = true;

        // rotate todo
        m_channel.n = QVector3D( m_x, m_y+m_h, 0.5 );
        m_channel.s = QVector3D( m_x, m_y-m_h, 0.5 );
        m_channel.w = QVector3D( m_x-m_w, m_y, 0.5 );
        m_channel.e = QVector3D( m_x+m_w, m_y, 0.5 );
    }

    else m_channel.diffuse = false;
}

void MonoSource::setPosition(QVector3D position)
{
    m_channel.c = position;
    m_x = position.x();
    m_y = position.y();
    m_z = position.z();
    update();
}

void MonoSource::setX(qreal x)
{
    m_x = x;    
    m_channel.c.setX( x );
    update();
}

void MonoSource::setY(qreal y)
{
    m_y = y;
    m_channel.c.setY( y );

    update();
}

void MonoSource::setZ(qreal z)
{
    m_z = z;
    m_channel.c.setZ( z );

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

void StereoSource::expose(WPNNode*)
{
    m_left->setExposePath   ( m_exp_path+"/left" );
    m_right->setExposePath  ( m_exp_path+"/right" );
}

void StereoSource::allocateCoeffs(QVector<Speaker*> const& speakerset)
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

void StereoSource::setZ(qreal z)
{
    m_z = z;
    m_left  ->setZ(z);
    m_right ->setZ(z);
}

//---------------------------------------------------------------------------------------------------------

Rooms::Rooms() : m_setup(nullptr) { SETTYPE( StreamType::Effect ) }

void Rooms::setSetup(RoomSetup* setup)
{
    if ( m_setup == setup ) return;
    m_setup = setup;
    emit setupChanged();
}

void Rooms::componentComplete()
{
    if ( m_setup ) { SETN_OUT ( m_setup->nspeakers()); }
    else
    {
        setActive   ( false );
        SETN_OUT    ( 0 );
    }
}

void Rooms::initialize(qint64 nsamples)
{        
    for ( const auto& node : m_subnodes )
    {
        auto source = qobject_cast<RoomSource*>(node);
        if ( ! source ) continue;

        source->allocateCoeffs(m_setup->speakers());

        // manage coefficents for static sources
        if ( source->fixed() )
            for ( quint16 ch = 0; ch < source->numOutputs(); ++ch )
                source->channel(ch).computeCoeffs();
    }
}

float** Rooms::preprocess(float** buf, qint64 nsamples)
{    
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

    StreamNode::applyGain(out, nout, nsamples, m_level);
    return out;
}

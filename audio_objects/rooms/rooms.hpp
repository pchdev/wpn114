#ifndef ROOMS_H
#define ROOMS_H

#include <src/audio/audio.hpp>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QQmlListProperty>

class SpeakerArea : public QObject
{
    Q_OBJECT
    Q_PROPERTY  ( qreal radius READ radius WRITE setRadius NOTIFY radiusChanged )
    Q_PROPERTY  ( qreal bias READ bias WRITE setBias NOTIFY biasChanged )
    Q_PROPERTY  ( qreal angle READ angle WRITE setAngle NOTIFY angleChanged )

    public:
    SpeakerArea ( );
    SpeakerArea ( qreal radius, qreal bias, qreal angle );
    SpeakerArea ( const SpeakerArea& copy );

    SpeakerArea& operator= ( SpeakerArea const& );
    bool operator!= ( SpeakerArea const& );

    qreal radius  ( ) const { return m_radius; }
    qreal bias    ( ) const { return m_bias; }
    qreal angle   ( ) const { return m_angle; }

    void setRadius  ( qreal radius );
    void setBias    ( qreal bias );
    void setAngle   ( qreal angle );

    signals:
    void radiusChanged  ( );
    void biasChanged    ( );
    void angleChanged   ( );

    private:
    qreal m_radius  = 0.5;
    qreal m_bias    = 0.5;
    qreal m_angle   = 0;
};

Q_DECLARE_METATYPE( SpeakerArea )

class Speaker : public QObject
{
    Q_OBJECT

    Q_PROPERTY  ( QVector3D position READ position WRITE setPosition NOTIFY positionChanged )
    Q_PROPERTY  ( qreal x READ x WRITE setX NOTIFY xChanged )
    Q_PROPERTY  ( qreal y READ y WRITE setY NOTIFY yChanged )
    Q_PROPERTY  ( qreal z READ z WRITE setZ NOTIFY zChanged )
    Q_PROPERTY  ( SpeakerArea* horizontalArea READ horizontalArea WRITE setHorizontalArea NOTIFY horizontalAreaChanged )
    Q_PROPERTY  ( SpeakerArea* verticalArea READ verticalArea WRITE setVerticalArea NOTIFY verticalAreaChanged )

    public:
    Speaker  ( );
    ~Speaker ( );

    Speaker ( QVector3D position );
    Speaker ( QVector3D position, SpeakerArea harea, SpeakerArea varea);
    Speaker ( Speaker const& );
    Speaker& operator=(Speaker const&);

    QVector3D position             ( ) const { return m_position; }
    SpeakerArea* horizontalArea    ( ) const { return m_horizontal_area; }
    SpeakerArea* verticalArea      ( ) const { return m_vertical_area; }

    qreal x() const { return m_position.x(); }
    qreal y() const { return m_position.y(); }
    qreal z() const { return m_position.z(); }

    void setX( qreal x );
    void setY( qreal y );
    void setZ( qreal z );

    void setPosition        ( QVector3D const& position );
    void setHorizontalArea  ( SpeakerArea* area );
    void setVerticalArea    ( SpeakerArea* area );

    signals:
    void positionChanged        ();
    void horizontalAreaChanged  ();
    void verticalAreaChanged    ();
    void xChanged ( );
    void yChanged ( );
    void zChanged ( );

    private:
    QVector3D m_position;
    SpeakerArea* m_horizontal_area;
    SpeakerArea* m_vertical_area;
};

class RoomNode : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES    ( QQmlParserStatus )
    Q_PROPERTY      ( qreal horizontalInfluence READ horizontalInfluence WRITE setHorizontalInfluence )
    Q_PROPERTY      ( qreal verticalInfluence READ verticalInfluence WRITE setVerticalInfluence )

    public:
    RoomNode();
    ~RoomNode();

    qreal horizontalInfluence() const { return m_h_influence; }
    qreal verticalInfluence() const { return m_v_influence; }

    void setHorizontalInfluence(qreal hinf);
    void setVerticalInfluence(qreal vinf);

    virtual void classBegin() override {}
    virtual void componentComplete() override;
    virtual QVector<Speaker*> const& getSpeakers( ) const { return m_speakers; }
    Q_INVOKABLE QVariant speakers() const;

    protected:
    QVector<Speaker*> m_speakers;
    qreal m_h_influence = 0.5;
    qreal m_v_influence = 0.5;
};

class SingleSpeaker : public RoomNode
{
    Q_OBJECT
    Q_PROPERTY  ( QVector3D position READ position WRITE setPosition )

    public:
    SingleSpeaker();

    QVector3D position() const { return m_speaker->position(); }
    void setPosition(QVector3D position) { m_speaker->setPosition(position); }

    private:
    Speaker* m_speaker = nullptr;
};

class SpeakerPair : public RoomNode
{
    Q_OBJECT
    Q_PROPERTY  ( qreal xspread READ xspread WRITE setXspread )
    Q_PROPERTY  ( qreal yspread READ yspread WRITE setYspread )
    Q_PROPERTY  ( qreal zspread READ zspread WRITE setZspread )
    Q_PROPERTY  ( qreal x READ x WRITE setX )
    Q_PROPERTY  ( qreal y READ y WRITE setY )
    Q_PROPERTY  ( qreal z READ z WRITE setZ )
    Q_PROPERTY  ( Speaker* left READ left )
    Q_PROPERTY  ( Speaker* right READ right )

    public:
    SpeakerPair();

    virtual void componentComplete() override;

    Speaker* left   () const { return m_left; }
    Speaker* right  () const { return m_right; }

    qreal xspread() const { return m_xspread; }
    qreal yspread() const { return m_yspread; }
    qreal zspread() const { return m_zspread; }
    qreal x() const { return m_x; }
    qreal y() const { return m_y; }
    qreal z() const { return m_z; }

    void setXspread( qreal xspread );
    void setYspread( qreal yspread );
    void setZspread( qreal zspread );
    void setX( qreal x );
    void setY( qreal y );
    void setZ( qreal z );

    private:
    Speaker* m_left   = nullptr;
    Speaker* m_right  = nullptr;
    qreal m_xspread   = 0.25;
    qreal m_yspread   = 0;
    qreal m_zspread   = 0;
    qreal m_x         = 0;
    qreal m_y         = 0;
    qreal m_z         = 0;

};

class SpeakerRing : public RoomNode
{
    Q_OBJECT

    Q_PROPERTY  ( int nspeakers READ nspeakers WRITE setNspeakers NOTIFY nspeakersChanged )
    Q_PROPERTY  ( qreal offset READ offset WRITE setOffset NOTIFY offsetChanged )
    Q_PROPERTY  ( qreal elevation READ elevation WRITE setElevation NOTIFY elevationChanged )
    Q_PROPERTY  ( qreal width READ width WRITE setWidth NOTIFY widthChanged )
    Q_PROPERTY  ( qreal height READ height WRITE setHeight NOTIFY heightChanged )
    Q_PROPERTY  ( qreal radius READ radius WRITE setRadius NOTIFY radiusChanged )

    public:
    SpeakerRing();

    virtual void componentComplete() override;

    qreal offset     ( ) const { return m_offset; }
    qreal elevation  ( ) const { return m_elevation; }
    qreal width      ( ) const { return m_width; }
    qreal height     ( ) const { return m_height; }
    qreal radius     ( ) const { return m_radius; }

    quint16 nspeakers( ) const { return m_speakers.size(); }
    void setNspeakers( quint16 nspeakers );

    void setOffset      ( qreal offset );
    void setElevation   ( qreal elevation );
    void setWidth       ( qreal width );
    void setHeight      ( qreal height );
    void setRadius      ( qreal radius );

    signals:
    void nspeakersChanged   ( );
    void offsetChanged      ( );
    void elevationChanged   ( );
    void widthChanged       ( );
    void heightChanged      ( );
    void radiusChanged      ( );

    private:
    void update();

    qreal m_width       = 1;
    qreal m_height      = 1;
    qreal m_elevation   = 0;
    qreal m_offset      = 0;
    qreal m_radius      = 1;
};

// x, y, z, w = influence
class RoomSetup : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_CLASSINFO ( "DefaultProperty", "nodes" )
    Q_PROPERTY  ( int nspeakers READ nspeakers )
    Q_PROPERTY  ( QQmlListProperty<RoomNode> nodes READ nodes NOTIFY nodesChanged )

    public:
    RoomSetup();
    ~RoomSetup();

    quint16 nspeakers() const { return m_speakers.size(); }

    virtual void classBegin() override {}
    virtual void componentComplete() override;

    QQmlListProperty<RoomNode> nodes    ( );
    const QVector<RoomNode*>& getNodes  ( ) const { return m_nodes; }
    QVector<Speaker*> speakers ( ) const { return m_speakers; }

    Q_INVOKABLE QVariantList speakerList() const;

    void appendNode     ( RoomNode* );
    int nodeCount       ( ) const;
    RoomNode* node      ( int ) const;
    void clearNodes     ( );

    signals:
    void nodesChanged();

    private:
    static void appendNode  ( QQmlListProperty<RoomNode>*, RoomNode*);
    static int nodeCount    ( QQmlListProperty<RoomNode>* );
    static RoomNode* node   ( QQmlListProperty<RoomNode>*, int );
    static void clearNodes  ( QQmlListProperty<RoomNode>* );

    QVector<RoomNode*> m_nodes;
    QVector<Speaker*> m_speakers;

};

struct RoomChannel
{
    qreal spgain ( QVector3D const& src, Speaker const& ls );
    void computeCoeffs ( );

    QVector3D c;
    QVector3D n;
    QVector3D w;
    QVector3D s;
    QVector3D e;

    QVector<Speaker*> speakers;

    float* coeffs   = nullptr;
    float  diffuse  = 0.5;
};

class RoomSource : public StreamNode
{
    Q_OBJECT

    Q_PROPERTY  ( qreal diffuse READ diffuse WRITE setDiffuse )
    Q_PROPERTY  ( qreal bias READ bias WRITE setBias )
    Q_PROPERTY  ( qreal rotate READ rotate WRITE setRotate )
    Q_PROPERTY  ( bool fixed READ fixed WRITE setFixed )
    Q_PROPERTY  ( int nchannels READ nchannels )
    Q_PROPERTY  ( qreal x READ x WRITE setX )
    Q_PROPERTY  ( qreal y READ y WRITE setY )
    Q_PROPERTY  ( qreal z READ z WRITE setZ )

    public:
    RoomSource();

    virtual void componentComplete() override;

    virtual float** preprocess ( float** buf, qint64 le ) override;
    virtual float** process ( float** buf, qint64 le ) override {}
    virtual void initialize ( qint64 ) override;

    virtual quint16 nchannels  ( ) const = 0;

    virtual void allocateCoeffs ( QVector<Speaker*> const& speakerset ) = 0;
    virtual RoomChannel& channel(quint16 channel) = 0;

    qreal x ( ) const { return m_x; }
    qreal y ( ) const { return m_y; }
    qreal z ( ) const { return m_z; }

    qreal diffuse   ( ) const { return m_diffuse; }
    qreal bias      ( ) const { return m_bias; }
    qreal rotate    ( ) const { return m_rotate; }
    bool fixed      ( ) const { return m_fixed; }

    virtual void setX           ( qreal x );
    virtual void setY           ( qreal y );
    virtual void setZ           ( qreal z );
    virtual void setDiffuse     ( qreal diffuse );
    virtual void setRotate      ( qreal rotate );
    virtual void setBias        ( qreal bias );
    virtual void setFixed       ( bool fixed );

    public slots:
    void onSingleSourceActiveChanged();

    protected:
    virtual void update() {}
    bool m_fixed;

    qreal m_x = 0.5;
    qreal m_y = 0.5;
    qreal m_z = 0.5;
    qreal m_diffuse = 0;
    qreal m_rotate = 0;
    qreal m_bias = 0.5;
};

class MonoSource : public RoomSource
{
    Q_OBJECT
    Q_PROPERTY  ( QVector3D position READ position WRITE setPosition NOTIFY positionChanged )

    public:
    MonoSource();

    virtual void componentComplete() override;

    virtual quint16 nchannels ( ) const override { return 1; }

    virtual void allocateCoeffs ( QVector<Speaker*> const& speakerset ) override;
    virtual RoomChannel& channel(quint16 channel) override;

    void setX   ( qreal x ) override;
    void setY   ( qreal y ) override;
    void setZ   ( qreal z ) override;

    QVector3D position  ( ) const { return m_channel.c; }
    void setPosition    ( QVector3D position );

    signals:
    void positionChanged();

    protected:
    void update() override;
    RoomChannel m_channel;
    qreal m_w;
    qreal m_h;

};

class StereoSource : public RoomSource
{
    Q_OBJECT

    Q_PROPERTY  ( qreal xspread READ xspread WRITE setXspread )
    Q_PROPERTY  ( qreal yspread READ yspread WRITE setYspread )
    Q_PROPERTY  ( MonoSource* left READ left )
    Q_PROPERTY  ( MonoSource* right READ right )

    public:
    StereoSource();
    ~StereoSource();

    virtual void componentComplete() override;
    virtual quint16 nchannels ( ) const override { return 2; }

    virtual void allocateCoeffs ( QVector<Speaker*> const& speakerset ) override;
    virtual RoomChannel& channel(quint16 channel) override;

    virtual void expose(WPNNode*) override;

    qreal xspread       ( ) const { return m_xspread; }
    qreal yspread       ( ) const { return m_yspread; }
    MonoSource* left    ( )  { return m_left; }
    MonoSource* right   ( ) { return m_right; }

    void setXspread     ( qreal xspread );
    void setYspread     ( qreal yspread );

    void setX           ( qreal x ) override;
    void setY           ( qreal y ) override;
    void setZ           ( qreal z ) override;
    void setDiffuse     ( qreal diffuse ) override;
    void setRotate      ( qreal rotate ) override;
    void setBias        ( qreal bias ) override;

    private:
    qreal m_xspread;
    qreal m_yspread;
    MonoSource* m_left;
    MonoSource* m_right;

};

class Rooms : public StreamNode
{
    Q_OBJECT

    Q_PROPERTY  ( RoomSetup* setup READ setup WRITE setSetup NOTIFY setupChanged )

    public:
    Rooms();

    RoomSetup* setup() const { return m_setup; }
    void setSetup(RoomSetup* setup);

    virtual void componentComplete() override;

    virtual float** preprocess  ( float** buf, qint64 le ) override;
    virtual float** process     ( float** buf, qint64 le ) override {}
    virtual void initialize     ( qint64 ) override;

    signals:
    void setupChanged();

    private:    
    RoomSetup* m_setup;
};

#endif // ROOMS_H

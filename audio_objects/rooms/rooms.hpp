#ifndef ROOMS_H
#define ROOMS_H

#include <src/audio/audio.hpp>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QQmlListProperty>

class RoomNode : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES    ( QQmlParserStatus )

    Q_PROPERTY  ( int nspeakers READ nspeakers WRITE setNspeakers )
    Q_PROPERTY  ( QVariant influence READ influence WRITE setInfluence )
    Q_PROPERTY  ( QVariant position READ position WRITE setPosition )

    public:
    RoomNode();

    virtual void classBegin() override {}
    virtual void componentComplete() override {}

    quint16 nspeakers   ( ) const { return m_nspeakers; }
    void setNspeakers   ( quint16 nspeakers );

    QVariant influence  ( ) const { return m_influence; }
    QVariant position   ( ) const { return m_position; }

    virtual void setInfluence   ( QVariant influence );
    virtual void setPosition    ( QVariant position );

    QVector<QVector3D> positions () const { return m_positions; }
    QVector<qreal> influences    () const { return m_influences; }

    QVector4D speakerData(quint16 index) const;

    protected:
    QVariant m_influence;
    QVariant m_position;
    QVector<qreal> m_influences;
    QVector<QVector3D> m_positions;
    quint16 m_nspeakers;
};

class SpeakerPair : public RoomNode
{
    Q_OBJECT
    Q_PROPERTY  ( qreal xspread READ xspread WRITE setXspread )
    Q_PROPERTY  ( qreal yspread READ yspread WRITE setYspread )
    Q_PROPERTY  ( qreal x READ x WRITE setX )
    Q_PROPERTY  ( qreal y READ y WRITE setY )

    public:
    SpeakerPair();

    virtual void componentComplete() override;

    qreal xspread() const { return m_xspread; }
    qreal yspread() const { return m_yspread; }
    qreal x() const { return m_x; }
    qreal y() const { return m_y; }

    void setXspread(qreal xspread);
    void setYspread(qreal yspread);
    void setX(qreal x);
    void setY(qreal y);

    private:
    qreal m_xspread;
    qreal m_yspread;
    qreal m_x;
    qreal m_y;

};

class SpeakerRing : public RoomNode
{
    Q_OBJECT

    Q_PROPERTY      ( qreal offset READ offset WRITE setOffset )
    Q_PROPERTY      ( qreal elevation READ elevation WRITE setElevation )
    Q_PROPERTY      ( qreal width READ width WRITE setWidth )
    Q_PROPERTY      ( qreal height READ height WRITE setHeight )

    public:
    SpeakerRing();

    virtual void componentComplete() override;

    qreal offset     ( ) const { return m_offset; }
    qreal elevation  ( ) const { return m_elevation; }
    qreal width      ( ) const { return m_width; }
    qreal height     ( ) const { return m_height; }

    void setOffset      ( qreal offset );
    void setElevation   ( qreal elevation );
    void setWidth       ( qreal width );
    void setHeight      ( qreal height );

    private:
    qreal m_width       = 1.f;
    qreal m_height      = 1.f;
    qreal m_elevation   = 0.f;
    qreal m_offset      = 0.f;
};

// x, y, z, w = influence
class RoomSetup : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_CLASSINFO ( "DefaultProperty", "nodes" )
    Q_PROPERTY  ( int nspeakers READ nspeakers )
    Q_PROPERTY  ( QQmlListProperty<RoomNode> nodes READ nodes )

    public:
    RoomSetup();
    quint16 nspeakers() const { return m_nspeakers; }

    virtual void classBegin() override {}
    virtual void componentComplete() override;

    QQmlListProperty<RoomNode> nodes    ( );
    const QVector<RoomNode*>& getNodes  ( ) const { return m_nodes; }
    const QVector<QVector4D> speakers   ( ) const;

    void appendNode     ( RoomNode* );
    int nodeCount       ( ) const;
    RoomNode* node      ( int ) const;
    void clearNodes     ( );

    private:
    static void appendNode  ( QQmlListProperty<RoomNode>*, RoomNode*);
    static int nodeCount    ( QQmlListProperty<RoomNode>* );
    static RoomNode* node   ( QQmlListProperty<RoomNode>*, int );
    static void clearNodes  ( QQmlListProperty<RoomNode>* );

    QVector<RoomNode*> m_nodes;
    quint16 m_nspeakers;
};

struct RoomChannel
{
    qreal spgain ( QVector3D src, QVector4D ls );
    void computeCoeffs ( );

    QVector3D c;
    QVector3D n;
    QVector3D w;
    QVector3D s;
    QVector3D e;

    QVector<QVector4D> speakers;

    float* coeffs;
    bool diffuse;
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

    public:
    RoomSource();

    virtual void componentComplete() override;

    virtual float** process ( float** buf, qint64 le ) override;
    virtual void initialize ( qint64 ) override;

    virtual quint16 nchannels  ( ) const = 0;

    virtual void allocateCoeffs ( QVector<QVector4D> const& speakerset ) = 0;
    virtual RoomChannel& channel(quint16 channel) = 0;

    qreal x ( ) const { return m_x; }
    qreal y ( ) const { return m_y; }

    qreal diffuse   ( ) const { return m_diffuse; }
    qreal bias      ( ) const { return m_bias; }
    qreal rotate    ( ) const { return m_rotate; }
    bool fixed      ( ) const { return m_fixed; }

    virtual void setX           ( qreal x );
    virtual void setY           ( qreal y );
    virtual void setDiffuse     ( qreal diffuse );
    virtual void setRotate      ( qreal rotate );
    virtual void setBias        ( qreal bias );
    virtual void setFixed       ( bool fixed );

    protected:
    virtual void update() {}
    bool m_fixed;

    qreal m_x = 0.5;
    qreal m_y = 0.5;
    qreal m_diffuse = 0;
    qreal m_rotate = 0;
    qreal m_bias = 0.5;
};

class MonoSource : public RoomSource
{
    Q_OBJECT
    Q_PROPERTY  ( QVector3D position READ position WRITE setPosition )

    public:
    MonoSource();

    virtual quint16 nchannels ( ) const override { return 1; }

    virtual void allocateCoeffs ( QVector<QVector4D> const& speakerset ) override;
    virtual RoomChannel& channel(quint16 channel) override;

    void setX   ( qreal x ) override;
    void setY   ( qreal y ) override;

    QVector3D position  ( ) const { return m_channel.c; }
    void setPosition    ( QVector3D position );

    protected:
    void update() override;
    RoomChannel m_channel;

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

    virtual quint16 nchannels ( ) const override { return 2; }

    virtual void allocateCoeffs ( QVector<QVector4D> const& speakerset ) override;
    virtual RoomChannel& channel(quint16 channel) override;

    qreal xspread       ( ) const { return m_xspread; }
    qreal yspread       ( ) const { return m_yspread; }
    MonoSource* left    ( )  { return m_left; }
    MonoSource* right   ( ) { return m_right; }

    void setXspread     ( qreal xspread );
    void setYspread     ( qreal yspread );

    void setX           ( qreal x ) override;
    void setY           ( qreal y ) override;
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

    Q_PROPERTY  ( RoomSetup* setup READ setup WRITE setSetup )

    public:
    Rooms();

    RoomSetup* setup() const { return m_setup; }
    void setSetup(RoomSetup* setup);

    virtual float** preprocess  ( float** buf, qint64 le ) override;
    virtual float** process     ( float** buf, qint64 le ) override {}
    virtual void initialize     ( qint64 ) override;

    private:    

    QVector<QVector4D> m_speakers;
    RoomSetup* m_setup;
};

#endif // ROOMS_H

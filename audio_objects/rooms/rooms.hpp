#ifndef ROOMS_H
#define ROOMS_H

#include <src/audio/audio.hpp>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QQmlListProperty>

class RoomNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY  ( int nspeakers READ nspeakers WRITE setNspeakers )
    Q_PROPERTY  ( QVariant influence READ influence WRITE setInfluence )
    Q_PROPERTY  ( QVariant position READ position WRITE setPosition )

    public:
    RoomNode();
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

class SpeakerRing : public RoomNode, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES    ( QQmlParserStatus )
    Q_PROPERTY      ( qreal offset READ offset WRITE setOffset )
    Q_PROPERTY      ( qreal elevation READ elevation WRITE setElevation )

    Q_PROPERTY      ( qreal width READ width WRITE setWidth )
    Q_PROPERTY      ( qreal height READ height WRITE setHeight )

    public:
    SpeakerRing();

    virtual void classBegin() override {}
    virtual void componentComplete() override;

    qreal offset    ( ) const { return m_offset; }
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

    QQmlListProperty<RoomNode> nodes        ( );
    const QVector<RoomNode*>& getNodes      ( ) const { return m_nodes; }
    const QVector<QVector4D> speakers       ( ) const;

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

using RoomChannel = QVector<QVector3D>;



class RoomSource : public StreamNode, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY  ( QVector3D position READ position WRITE setPosition )
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
    virtual void classBegin() override {}

    virtual float** userProcess ( float** buf, qint64 le ) override;
    virtual void userInitialize ( qint64 ) override;

    virtual quint16 nchannels   ( ) const { return 1; }

    qreal x ( ) const { return m_x; }
    qreal y ( ) const { return m_y; }

    QVector3D position  ( ) const { return m_position; }
    qreal diffuse       ( ) const { return m_diffuse; }
    qreal bias          ( ) const { return m_bias; }
    qreal rotate        ( ) const { return m_rotate; }
    bool fixed          ( ) const { return m_fixed; }

    virtual void setX           ( qreal x );
    virtual void setY           ( qreal y );
    virtual void setPosition    ( QVector3D position );
    virtual void setDiffuse     ( qreal diffuse );
    virtual void setRotate      ( qreal rotate );
    virtual void setBias        ( qreal bias );
    virtual void setFixed       ( bool fixed );

    protected:
    void update();

    bool m_fixed;
    qreal m_x = 0.5;
    qreal m_y = 0.5;
    QVector3D m_position;
    qreal m_diffuse = 0;
    qreal m_rotate = 0;
    qreal m_bias = 0.5;
    QVector3D m_extremities[4];
};

class RoomStereoSource : public RoomSource
{
    Q_OBJECT

    Q_PROPERTY  ( qreal xspread READ xspread WRITE setXspread )
    Q_PROPERTY  ( qreal yspread READ yspread WRITE setYspread )
    Q_PROPERTY  ( RoomSource* left READ left )
    Q_PROPERTY  ( RoomSource* right READ right )

    public:
    RoomStereoSource();

    qreal xspread       ( ) const { return m_xspread; }
    qreal yspread       ( ) const { return m_yspread; }
    RoomSource* left    ( )  { return m_left; }
    RoomSource* right   ( ) { return m_right; }

    void setX           ( qreal x ) override;
    void setY           ( qreal y ) override;
    void setXspread     ( qreal xspread );
    void setYspread     ( qreal yspread );
    void setDiffuse     ( qreal diffuse ) override;
    void setRotate      ( qreal rotate ) override;
    void setBias        ( qreal bias ) override;
    void setPosition    ( QVector3D  ) override {}

    private:
    qreal m_xspread;
    qreal m_yspread;
    RoomSource* m_left;
    RoomSource* m_right;

};

class Rooms : public StreamNode
{
    Q_OBJECT

    Q_PROPERTY  ( RoomSetup* setup READ setup WRITE setSetup )

    public:
    Rooms();

    RoomSetup* setup() const { return m_setup; }
    void setSetup(RoomSetup* setup);

    virtual float** process ( float** buf, qint64 le ) override;
    virtual float** userProcess ( float** buf, qint64 le ) override;
    virtual void userInitialize ( qint64 ) override;

    private:
    void computeCoeffs(RoomSource& source);
    qreal spgain (QVector3D src, QVector4D ls);

    QVector<QVector4D> m_speakers;
    RoomSetup* m_setup;

};

#endif // ROOMS_H

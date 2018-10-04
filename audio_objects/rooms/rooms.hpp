#ifndef ROOMS_H
#define ROOMS_H

#include <src/audio/audio.hpp>
#include <QVector3D>
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

    QVector<QVector3D> positions() const { return m_positions; }
    QVector<qreal> influences() const { return m_influences; }

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
    Q_PROPERTY      ( int offset READ offset WRITE setOffset )
    Q_PROPERTY      ( qreal elevation READ elevation WRITE setElevation )

    Q_PROPERTY      ( qreal width READ width WRITE setWidth )
    Q_PROPERTY      ( qreal height READ height WRITE setHeight )

    public:
    SpeakerRing();

    virtual void classBegin() override {}
    virtual void componentComplete() override;

    qint16 offset    ( ) const { return m_offset; }
    qreal elevation  ( ) const { return m_elevation; }
    qreal width      ( ) const { return m_width; }
    qreal height     ( ) const { return m_height; }

    void setOffset      ( qint16 offset );
    void setElevation   ( qreal elevation );
    void setWidth       ( qreal width );
    void setHeight      ( qreal height );

    private:
    qreal m_width       = 1.f;
    qreal m_height      = 1.f;
    qreal m_elevation   = 0.f;
    qint16 m_offset     = 0;
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

    QQmlListProperty<RoomNode> nodes();
    const QVector<RoomNode*>& getNodes() const { return m_nodes; }

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

class RoomSource : public StreamNode
{
    Q_OBJECT

    Q_PROPERTY  ( QVariant position READ position WRITE setPosition )
    Q_PROPERTY  ( QVariant diffuse READ diffuse WRITE setDiffuse )
    Q_PROPERTY  ( QVariant bias READ bias WRITE setBias )
    Q_PROPERTY  ( QVariant rotate READ rotate WRITE setRotate )
    Q_PROPERTY  ( int nchannels READ nchannels )

    public:
    RoomSource();

    virtual float** userProcess ( float** buf, qint64 le ) override;
    virtual void userInitialize ( qint64 ) override;

    QVariant position   ( ) const { return m_position; }
    QVariant diffuse    ( ) const { return m_diffuse; }
    QVariant bias       ( ) const { return m_bias; }
    QVariant rotate     ( ) const { return m_rotate; }
    quint16 nchannels   ( ) const { return m_nchannels; }

    QVector<RoomChannel> const& channels() const { return m_channels; }

    void setPosition    ( QVariant position );
    void setDiffuse     ( QVariant diffuse );
    void setRotate      ( QVariant rotate );
    void setBias        ( QVariant bias );

    private:    
    void update();

    QVector<RoomChannel> m_channels;
    quint16 m_nchannels;
    QVariant m_position;
    QVariant m_diffuse;
    QVariant m_rotate;
    QVariant m_bias;
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
    RoomSetup* m_setup;

};

#endif // ROOMS_H

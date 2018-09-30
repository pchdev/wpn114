#ifndef ROOMS_H
#define ROOMS_H

#include <src/audio/audio.hpp>

class RoomNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY  ( int nspeakers READ nspeakers WRITE setNspeakers )

    public:
    RoomNode();
    quint16 nspeakers() const { return m_nspeakers; }
    void setNspeakers(quint16 nspeakers);

    QVector<QVector3D> positions() const { return m_positions; }
    QVector<qreal> influences() const { return m_infuences; }

    protected:
    QVector<QVector3D> m_positions;
    QVector<qreal> m_infuences;
    quint16 m_nspeakers;
};

class CircularSetup : public RoomNode
{
    Q_OBJECT
    Q_PROPERTY  ( int offset READ offset WRITE setOffset )

    public:
    CircularSetup();
    qint16 offset() const { return m_offset; }

    void setOffset(qint16 offset);

    private:
    qint16 m_offset;

};

class RoomSetup : public QObject
{
    Q_OBJECT

    public:
    RoomSetup();
};

class RoomSource : public StreamNode
{
    Q_OBJECT

    Q_PROPERTY  ( QVariant position READ position WRITE setPosition )
    Q_PROPERTY  ( QVariant diffuse READ diffuse WRITE setDiffuse )
    Q_PROPERTY  ( QVariant bias READ bias WRITE setBias )

    public:
    RoomSource();

    virtual float** userProcess ( float** buf, qint64 le ) override;
    virtual void userInitialize ( qint64 ) override;

    QVariant position   ( ) const { return m_position; }
    QVariant diffuse    ( ) const { return m_diffuse; }
    QVariant bias       ( ) const { return m_bias; }

    void setPosition    ( QVariant position );
    void setDiffuse     ( QVariant diffuse );
    void setBias        ( QVariant bias );

    private:
    QVariant m_position;
    QVariant m_diffuse;
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

#pragma once

#include <QObject>
#include <QQmlParserStatus>
#include <QJsonObject>

class QueryNode : public QObject, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY  ( QString name READ name WRITE setName NOTIFY nameChanged )
    Q_PROPERTY  ( QString address READ address WRITE setAddress NOTIFY addressChanged )
    Q_PROPERTY  ( QVariant value READ value WRITE setValue NOTIFY valueChanged )
    Q_PROPERTY  ( QueryNode::Type type READ type WRITE setType )
    Q_PROPERTY  ( bool critical READ critical WRITE setCritical )
    Q_PROPERTY  ( OSCQueryDevice* device READ device WRITE setDevice NOTIFY deviceChanged )
    Q_PROPERTY  ( QueryNode * parent READ parent WRITE setParent NOTIFY parentChanged )

    public:
    QueryNode();
    ~QueryNode();

    enum class Type
    {
        None        = 0,
        Bool        = 1,
        Int         = 2,
        Float       = 3,
        String      = 4,
        List        = 5,
        Vec2f       = 6,
        Vec3f       = 7,
        Vec4f       = 8,
        Char        = 9,
        Impulse     = 10,
    };

    Q_ENUM  ( Type )

    virtual void componentComplete();
    virtual void classBegin();

    QString name                ( ) const { return m_name; }
    QString address             ( ) const { return m_address; }
    QVariant value              ( ) const { return m_value; }
    bool critical               ( ) const { return m_critical; }
    OSCQueryDevice* device      ( ) const { return m_device; }
    QueryNode::Type type        ( ) const { return m_type; }
    QString typeString          ( ) const;
    QueryNode* parent           ( ) { return m_parent; }

    QVector<QueryNode*> getChildren ( ) const { return m_children; }

    QJsonObject info    ( ) const;
    void post           ( ) const;

    void setName        ( QString name );
    void setAddress     ( QString address );
    void setValue       ( QVariant value );
    void setValueQuiet  ( QVariant value );
    void setCritical    ( bool critical );
    void setDevice      ( OSCQueryDevice* device );
    void setType        ( QueryNode::Type type );
    void setParent      ( QueryNode* parent );

    QueryNode* getChild ( QString child );
    QueryNode* getChild ( uint64_t index );

    void addChild       ( QueryNode* node );
    void addChild       ( QString name );
    void removeChild    ( QueryNode* node );
    void removeChild    ( QString name );

    signals:
    void nameChanged        ( QString );
    void addressChanged     ( QString );
    void valueChanged       ( QVariant );
    void valueReceived      ( );
    void deviceChanged      ( OSCQueryDevice* );
    void parentChanged      ( );

    private:
    QJsonValue valueJson    () const;

    QString                 m_name;
    QString                 m_address;
    QueryNode::Type         m_type;
    QVariant                m_value;
    OSCQueryDevice*         m_device;
    bool                    m_critical;
    QVector<QueryNode*>     m_children;
    QueryNode*              m_parent;

};

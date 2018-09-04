#pragma once

#include <QObject>
#include <QQmlParserStatus>
#include <QJsonObject>
#include "device.hpp"
#include <QVariant>
#include <QVector>

class OSCQueryDevice;

class QueryNode : public QObject, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY  ( QString name READ name WRITE setName NOTIFY nameChanged )
    Q_PROPERTY  ( QString path READ path WRITE setPath NOTIFY pathChanged )
    Q_PROPERTY  ( QVariant value READ value WRITE setValue NOTIFY valueChanged )
    Q_PROPERTY  ( QueryNode::Type type READ type WRITE setType )
    Q_PROPERTY  ( bool critical READ critical WRITE setCritical )
    Q_PROPERTY  ( OSCQueryDevice* device READ device WRITE setDevice NOTIFY deviceChanged )
    Q_PROPERTY  ( QueryNode* parent READ parent WRITE setParent NOTIFY parentChanged )

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

    QString name                 ( ) const { return m_name; }
    QString path              ( ) const { return m_path; }
    QVariant value               ( ) const { return m_value; }
    bool critical                ( ) const { return m_critical; }
    OSCQueryDevice* device       ( ) const { return m_device; }
    QueryNode::Type type         ( ) const { return m_type; }
    QVector<QueryNode*> subnodes ( ) const { return m_children; }
    QueryNode* parent            ( )       { return m_parent; }
    QString typeString           ( )        const;

    QJsonObject info        ( ) const;
    void post               ( ) const;

    void setName            ( QString name );
    void setPath            ( QString path );
    void setValue           ( QVariant value );
    void setValueQuiet      ( QVariant value );
    void setCritical        ( bool critical );
    void setDevice          ( OSCQueryDevice* device );
    void setType            ( QueryNode::Type type );
    void setParent          ( QueryNode* parent );

    QueryNode* subnode      (QString path );
    QueryNode* subnode      ( uint64_t index );

    void addSubnode         ( QueryNode* node );
    QueryNode *createSubnode( QString name );
    void removeSubnode      ( QueryNode* node );
    void removeSubnode      ( QString name );

    signals:
    void nameChanged        ( QString );
    void pathChanged     ( QString );
    void valueChanged       ( QVariant );
    void valueReceived      ( );
    void deviceChanged      ( OSCQueryDevice* );
    void parentChanged      ( );

    private:
    QJsonValue valueJson    () const;

    QString                 m_name;
    QString                 m_path;
    QueryNode::Type         m_type;
    QVariant                m_value;
    OSCQueryDevice*         m_device;
    bool                    m_critical;
    QVector<QueryNode*>     m_children;
    QueryNode*              m_parent;

};

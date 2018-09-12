#pragma once

#include <QQmlParserStatus>
#include <QJsonObject>
#include <QVariant>
#include <QVector>
#include "device.hpp"

class WPNDevice;

class WPNNode : public QObject, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY  ( QString name READ name WRITE setName NOTIFY nameChanged )
    Q_PROPERTY  ( QString path READ path WRITE setPath NOTIFY pathChanged )
    Q_PROPERTY  ( QVariant value READ value WRITE setValue NOTIFY valueChanged )
    Q_PROPERTY  ( WPNNode::Type type READ type WRITE setType )
    Q_PROPERTY  ( bool critical READ critical WRITE setCritical )
    Q_PROPERTY  ( WPNDevice* device READ device WRITE setDevice NOTIFY deviceChanged )
    Q_PROPERTY  ( WPNNode* parent READ parent WRITE setParent NOTIFY parentChanged )

    public:
    WPNNode();
    ~WPNNode();

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

    virtual void componentComplete  ( );
    virtual void classBegin         ( ) { }

    QJsonObject attribute   ( QString attr ) const;

    QString name            ( ) const { return m_name; }
    QString path            ( ) const { return m_path; }
    QVariant value          ( ) const { return m_value; }
    bool critical           ( ) const { return m_critical; }
    WPNDevice* device       ( ) const { return m_device; }
    WPNNode::Type type      ( ) const { return m_type; }
    WPNNode* parent         ( )       { return m_parent; }
    QString typeString      ( ) const;
    QJsonObject info        ( ) const;
    void post               ( ) const;

    void setName            ( QString name );
    void setPath            ( QString path );
    void setValue           ( QVariant value );
    void setValueQuiet      ( QVariant value );
    void setCritical        ( bool critical );
    void setDevice          ( WPNDevice* device );
    void setType            ( WPNNode::Type type );
    void setParent          ( WPNNode* parent );

    WPNNode* subnode                ( QString path );
    WPNNode* subnode                ( uint64_t index );
    QVector<WPNNode*> subnodes      ( ) const { return m_children; }
    WPNNode*createSubnode           ( QString name );
    void addSubnode                 ( WPNNode* node );
    void removeSubnode              ( WPNNode* node );
    void removeSubnode              ( QString name );

    bool listening          ( ) const { return m_listening; }
    void setListening       ( bool listen, WPNDevice* target );

    signals:
    void nameChanged        ( QString );
    void pathChanged        ( QString );
    void valueChanged       ( QVariant );
    void valueReceived      ( );
    void deviceChanged      ( WPNDevice* );
    void parentChanged      ( );

    private:
    QJsonValue valueJson    () const;

    QString             m_name;
    QString             m_path;
    WPNNode::Type       m_type;
    QVariant            m_value;
    WPNDevice*          m_device;
    WPNNode*            m_parent;

    bool  m_critical;
    bool  m_listening;

    QVector<WPNNode*>      m_children;
    QVector<WPNDevice*>    m_listeners;

};

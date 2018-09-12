#pragma once

#include <QQmlParserStatus>
#include <QJsonObject>
#include <QVariant>
#include <QVector>
#include "device.hpp"

class WPNDevice;

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

struct Range
{
    QVariant min;
    QVariant max;
    QVariantList vals;
};

enum class Clipmode
{
    NONE    = 0,
    LOW     = 1,
    HIGH    = 2,
    BOTH    = 3,
};

enum class Access
{
    NONE    = 0,
    READ    = 1,
    WRITE   = 2,
    RW      = 3
};

struct Attributes
{
    QString         path;
    Type            type;
    Access          access;
    QVariant        value;
    Range           range;
    QString         description;
    QStringList     tags;
    bool            critical;
    Clipmode        clipmode;
};

class WPNNode : public QObject, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY  ( QString name READ name WRITE setName )
    Q_PROPERTY  ( WPNDevice* device READ device WRITE setDevice )
    Q_PROPERTY  ( WPNNode* parent READ parent WRITE setParent )

    Q_PROPERTY  ( QString path READ path WRITE setPath )
    Q_PROPERTY  ( Type type READ type WRITE setType )
    Q_PROPERTY  ( Access access READ access WRITE setAccess )
    Q_PROPERTY  ( QVariant value READ value WRITE setValue NOTIFY valueChanged )
    Q_PROPERTY  ( Range range READ range WRITE setRange )
    Q_PROPERTY  ( QString description READ description WRITE setDescription )
    Q_PROPERTY  ( QStringList tags READ tags WRITE setTags )
    Q_PROPERTY  ( bool critical READ critical WRITE setCritical )
    Q_PROPERTY  ( Clipmode clipmode READ clipmode WRITE setClipmode )

    public:
    WPNNode();
    ~WPNNode();

    virtual void componentComplete  ( );
    virtual void classBegin         ( ) { }

    QString name            ( ) const { return m_name; }
    WPNDevice* device       ( ) const { return m_device; }
    WPNNode* parent         ( ) { return m_parent; }
    void post               ( ) const;

    void setName    ( QString name );
    void setDevice  ( WPNDevice* device );
    void setParent  ( WPNNode* parent );

    // attributes & json -------------------------------------------------

    QJsonObject attributeJson     ( QString attr ) const;
    QJsonObject attributesJson    ( ) const;
    const Attributes& attributes  ( ) const;
    QJsonValue jsonValue          ( ) const;
    QString typeTag               ( ) const;
    QJsonObject toJson            ( ) const;

    // attributes --------------------------------------------------------

    QString path            ( ) const { return m_attributes.path; }
    Type type               ( ) const { return m_attributes.type; }
    Access access           ( ) const { return m_attributes.access; }
    QVariant value          ( ) const { return m_attributes.value; }
    Range range             ( ) const { return m_attributes.range; }
    QString description     ( ) const { return m_attributes.description; }
    QStringList tags        ( ) const { return m_attributes.tags; }
    bool critical           ( ) const { return m_attributes.critical; }
    Clipmode clipmode       ( ) const { return m_attributes.clipmode; }

    void setPath            ( QString path );
    void setType            ( Type type );
    void setAccess          ( Access access );
    void setValue           ( QVariant value );
    void setValueQuiet      ( QVariant value );
    void setRange           ( Range range );
    void setDescription     ( QString description );
    void setTags            ( QStringList tags );
    void setCritical        ( bool critical );    
    void setClipmode        ( Clipmode clipmode );

    // tree/hierarchy ----------------------------------------------------

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
    void valueChanged       ( QVariant );
    void valueReceived      ( );

    private:
    Attributes      m_attributes;
    QString         m_name;
    WPNDevice*      m_device;
    WPNNode*        m_parent;

    bool  m_listening;

    QVector<WPNNode*>      m_children;
    QVector<WPNDevice*>    m_listeners;

};

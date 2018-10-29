#include "node.hpp"
#include <QtDebug>
#include <QJsonArray>
#include "folder.hpp"

#include <QQmlEngine>

WPNNode* WPNNode::fromJson(QJsonObject obj, WPNDevice *dev)
{
    WPNNode* node = nullptr;

    if ( obj["EXTENDED_TYPE"].toString() == "folder" )
         node = new WPNFolderMirror;
    else node = new WPNNode;

    node->setDevice ( dev );
    node->update ( obj );
    return node;
}

void WPNNode::update(QJsonObject obj)
{
    setPath           ( obj["FULL_PATH"].toString() );
    setTypeFromTag    ( obj["TYPE"].toString() );

    // recursively parse children
    if ( obj.contains("CONTENTS") )
    {
        QJsonObject contents = obj["CONTENTS"].toObject();

        for ( const auto& key : contents.keys() )
        {
            auto node = WPNNode::fromJson(contents[key].toObject(), m_device);
            node->setName ( key );
            addSubnode ( node );
        }
    }

    setExtendedType   ( obj["EXTENDED_TYPE"].toString());
    setAccess         ( static_cast<Access::Values>(obj["ACCESS"].toInt()) );
    setDescription    ( obj["DESCRIPTION"].toString()) ;
    setValue          ( obj["VALUE"].toVariant() );

}

WPNNode::WPNNode() :
    m_device(nullptr), m_parent(nullptr), m_target_property(nullptr), m_target(nullptr)
{
    m_attributes.access         = Access::NONE;
    m_attributes.clipmode       = Clipmode::NONE;
    m_attributes.type           = Type::None;
    m_attributes.description    = "No description";
    m_attributes.critical       = false;

    // prevents qml from destroying nodes when referenced in javascript functions
    QQmlEngine::setObjectOwnership( this, QQmlEngine::CppOwnership );

}

WPNNode::~WPNNode()
{
    for ( const auto& subnode : m_children )
          delete subnode;

    m_parent->removeSubnode(this);
}

void WPNNode::componentComplete()
{
    if ( m_name.isEmpty() )
        m_name = m_attributes.path.split('/').last();

    if ( m_attributes.type != Type::None )
         m_attributes.access = Access::RW;

    if ( m_target_property.isValid() )
    {
        // workaround for incorrect initialization of some js 'array' properties
        QVariant v = m_target_property.read();
        if ( QString(v.typeName()) == "QJSValue")
            propertyChanged();
    }

    if ( !m_device ) m_device = WPNDevice::instance();
    if ( m_device && !m_parent ) m_device->link(this);
}

void WPNNode::setAccess         ( Access::Values access ) { m_attributes.access = access; }
void WPNNode::setClipmode       ( Clipmode::Values mode ) { m_attributes.clipmode = mode; }
void WPNNode::setCritical       ( bool critical ) { m_attributes.critical = critical; }
void WPNNode::setDescription    ( QString description ) { m_attributes.description = description; }
void WPNNode::setTags           ( QStringList tags ) { m_attributes.tags = tags; }
void WPNNode::setRange          ( Range range ) { m_attributes.range = range; }

void WPNNode::setName           ( QString name ) { m_name = name; }
void WPNNode::setDevice         ( WPNDevice* device) { m_device = device; }

void WPNNode::setParent(WPNNode* parent)
{
    m_parent = parent;
    if ( !m_device ) m_device = parent->device();
}

qint16 WPNNode::index() const
{
    for ( quint16 i = 0; i < m_parent->subnodes().size(); ++i )
        if ( m_parent->subnodeAt(i) == this )
             return i;

    return -1;
}

QString WPNNode::parentPath() const
{
    auto spl = m_attributes.path.split('/');
    spl.removeLast();
    return spl.join('/');
}

QJsonObject WPNNode::attributesJson() const
{
    QJsonObject obj;

    obj.insert("FULL_PATH", m_attributes.path);
    obj.insert("ACCESS", static_cast<quint8>(m_attributes.access));    

    if  ( m_attributes.type != Type::None )
    {
        //obj.insert("DESCRIPTION", m_attributes.description);
        obj.insert("VALUE", jsonValue());
        obj.insert("CRITICAL", m_attributes.critical);
        obj.insert("TYPE", typeTag());

        if ( !m_attributes.extended_type.isEmpty() )
            obj.insert("EXTENDED_TYPE", m_attributes.extended_type );

    }

    //obj.insert("TAGS", m_attributes.tags);
    //obj.insert("RANGE");
    // + clipmode todo

    return obj;
}

QJsonObject WPNNode::attributeJson(QString attr) const
{
    QJsonObject obj;

    if ( attr == "VALUE" ) obj.insert(attr, QJsonArray{jsonValue()});
    return obj;
}

void WPNNode::setTypeFromMetaType(int type)
{
    switch(type)
    {
    case QMetaType::Bool: m_attributes.type = Type::Bool; break;
    case QMetaType::Float: m_attributes.type = Type::Float; break;
    case QMetaType::Double: m_attributes.type = Type::Float; break;
    case QMetaType::Int: m_attributes.type = Type::Int; break;
    case QMetaType::QString: m_attributes.type = Type::String; break;
    case QMetaType::QVector2D: m_attributes.type = Type::Vec2f; break;
    case QMetaType::QVector3D: m_attributes.type = Type::Vec3f; break;
    case QMetaType::QVector4D: m_attributes.type = Type::Vec4f; break;
    case QMetaType::QVariantList: m_attributes.type = Type::List; break;
    case QMetaType::QVariant: m_attributes.type = Type::List; break;
    default: m_attributes.type = Type::List; break;
    }
}

void WPNNode::setTarget(const QQmlProperty& property)
{
    m_target_property = property;    
    setTypeFromMetaType(property.propertyType());

    m_attributes.value = property.read();
    m_target_property.connectNotifySignal(this, SLOT(propertyChanged()));
}

void WPNNode::setTarget(QObject* sender, const QMetaProperty& property)
{
    m_meta_property = property;
    m_target = sender;

    setTypeFromMetaType(property.type());

    m_attributes.value = property.read(sender);

    auto mmcount = metaObject()->methodCount();
    QMetaMethod mmethod;

    for ( int i = 0; i < mmcount; ++i )
        if ( metaObject()->method(i).name() == QString("metaPropertyChanged"))
            mmethod = metaObject()->method(i);

    if ( property.hasNotifySignal() )
        QObject::connect(sender, property.notifySignal(), this, mmethod );
}

void WPNNode::metaPropertyChanged()
{
    QVariant value = m_meta_property.read(m_target);
    emit valueReceived(value);

    if ( value != m_attributes.value )
    {
        emit valueChanged(value);
        m_attributes.value = value;
    }

    for ( const auto& listener : m_listeners )
         listener->pushNodeValue(this);
}

void WPNNode::propertyChanged()
{
    // no need to push it back to qmlproperty
    QVariant value = m_target_property.read();
    emit valueReceived(value);

    if ( value != m_attributes.value )
    {
        emit valueChanged(value);
        m_attributes.value = value;
    }

    for ( const auto& listener : m_listeners )
         listener->pushNodeValue(this);
}

QString WPNNode::typeTag() const
{
    switch ( m_attributes.type )
    {
    case Type::Values::Bool:         return "T";
    case Type::Values::Char:         return "c";
    case Type::Values::Float:        return "f";
    case Type::Values::Impulse:      return "I";
    case Type::Values::Int:          return "i";
    case Type::Values::List:         return "";
    case Type::Values::None:         return "null";
    case Type::Values::String:       return "s";
    case Type::Values::Vec2f:        return "ff";
    case Type::Values::Vec3f:        return "fff";
    case Type::Values::Vec4f:        return "ffff";
    }

    return "";
}

void WPNNode::setTypeFromTag(QString tag)
{
    if ( tag == "f" ) m_attributes.type = Type::Float;
    else if ( tag == "T" || tag == "F" ) m_attributes.type = Type::Bool;
    else if ( tag == "i" ) m_attributes.type = Type::Int;
    else if ( tag == "" ) m_attributes.type = Type::List;
    else if ( tag == "I") m_attributes.type = Type::Impulse;
    else if ( tag == "s") m_attributes.type = Type::String;
    else if ( tag == "ff") m_attributes.type = Type::Vec2f;
    else if ( tag == "fff") m_attributes.type = Type::Vec3f;
    else if ( tag == "ffff") m_attributes.type = Type::Vec4f;
}

QJsonValue WPNNode::jsonValue() const
{
    QJsonValue v;
    switch ( m_attributes.type )
    {
    case Type::Values::Bool:      v = m_attributes.value.toBool(); break;
    case Type::Values::Char:      v = m_attributes.value.toString(); break;
    case Type::Values::Float:     v = m_attributes.value.toDouble(); break;
    case Type::Values::String:    v = m_attributes.value.toString(); break;
    case Type::Values::Int:       v = m_attributes.value.toInt(); break;
    case Type::Values::None:      return v;
    case Type::Values::Impulse:   return v;

    case Type::Values::List:    v = QJsonArray::fromVariantList(m_attributes.value.toList()); break;
    case Type::Values::Vec2f:   v = QJsonArray::fromVariantList(m_attributes.value.toList()); break;
    case Type::Values::Vec3f:   v = QJsonArray::fromVariantList(m_attributes.value.toList()); break;
    case Type::Values::Vec4f:   v = QJsonArray::fromVariantList(m_attributes.value.toList()); break;
    }

    return v;
}

QJsonObject WPNNode::toJson() const
{
    auto attr = attributesJson();
    if ( m_children.empty() ) return attr;

    QJsonObject contents;

    for ( const auto& child : m_children )
        contents.insert(child->name(), child->toJson());

    attr.insert("CONTENTS", contents);
    return attr;
}

void WPNNode::post() const
{
    qDebug() << toJson();

    for ( const auto& child : m_children )
        child->post();
}

void WPNNode::setPath(QString path)
{
    m_attributes.path = path;
}

void WPNNode::setValueQuiet(QVariant value)
{   
    if ( m_target_property.isWritable())
        m_target_property.write(value);

    if ( m_target )
    {
        //m_meta_property.write(m_target, value);
        m_target->setProperty(m_meta_property.name(), value);
    }

    emit valueReceived(value);

    if ( m_attributes.value != value )
    {
        m_attributes.value = value;
        emit valueChanged(value);
    }
}

void WPNNode::setValue(QVariant value)
{    
    setValueQuiet(value);

    for ( const auto& listener : m_listeners )
         listener->pushNodeValue(this);
}

void WPNNode::setType(Type::Values type)
{
    m_attributes.type = type;

    if ( type == Type::Values::List )
        m_attributes.extended_type = "list";

    else if ( type == Type::Values::Vec2f ||
              type == Type::Values::Vec3f ||
              type == Type::Values::Vec4f )
        m_attributes.extended_type = "vecf";
}

void WPNNode::setExtendedType(QString type)
{
    m_attributes.extended_type = type;
}

void WPNNode::setListening(bool listen, WPNDevice *target)
{
    if ( listen && !m_listeners.contains(target) )
        m_listeners.push_back(target);

    else if ( !listen ) m_listeners.removeAll(target);
}

void WPNNode::addSubnode(WPNNode *node)
{    
    if ( m_children.contains(node)) return;

    if ( node->name().isEmpty())
        node->setName("undefined");

    if ( node->path().isEmpty() )
         node->setPath(m_attributes.path +"/"+node->name());

    node->setParent         ( this );
    node->setDevice         ( m_device );
    m_children.push_back    ( node );
}

WPNNode* WPNNode::createSubnode(QString name)
{
    auto node = new WPNNode;

    if ( !path().endsWith("/") ) node->setPath( path()+"/"+name );
    else node->setPath( path()+name );

    node->setName       ( name );
    node->setParent     ( this );

    m_children.push_back ( node );

    return node;
}

void WPNNode::removeSubnode(WPNNode *node)
{
    m_children.removeAll(node);
}

void WPNNode::removeSubnode(QString name)
{

}

WPNNode* WPNNode::subnode(QString path)
{
    for ( const auto& child : m_children )
        if ( child->path() == path )
            return child;

    for ( const auto& child : m_children )
        if ( auto sub = child->subnode(path) )
            return sub;

    if ( path == m_attributes.path ) return this;

    return nullptr;
}

WPNNode* WPNNode::subnodeAt(int index)
{
    if ( index >= m_children.size() ) return nullptr;
    else return m_children[index];
}

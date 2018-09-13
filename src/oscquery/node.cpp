#include "node.hpp"
#include <QtDebug>
#include <QJsonArray>

WPNNode::WPNNode() : m_device(nullptr), m_parent(nullptr)
{
    m_attributes.access         = Access::RW;
    m_attributes.clipmode       = Clipmode::NONE;
    m_attributes.description    = "No description";
    m_attributes.critical       = false;
}

WPNNode::~WPNNode() { }

void WPNNode::componentComplete()
{
    if ( m_attributes.path.isEmpty() )
         m_attributes.path = "/"+ m_name;

    m_name = m_attributes.path.split('/').last();

    if      ( m_device ) m_device->addNode(m_device, this);
    else if ( m_parent )
    {
        m_device = m_parent->device();
        m_device->addNode(m_device, this);
    }
}

void WPNNode::setAccess         ( Access::Values access ) { m_attributes.access = access; }
void WPNNode::setClipmode       ( Clipmode::Values mode ) { m_attributes.clipmode = mode; }
void WPNNode::setCritical       ( bool critical ) { m_attributes.critical = critical; }
void WPNNode::setDescription    ( QString description ) { m_attributes.description = description; }
void WPNNode::setTags           ( QStringList tags ) { m_attributes.tags = tags; }
void WPNNode::setRange          ( Range range ) { m_attributes.range = range; }

void WPNNode::setName           ( QString name ) { m_name = name; }
void WPNNode::setDevice         ( WPNDevice* device) { m_device = device; }
void WPNNode::setParent         ( WPNNode* parent ) { m_parent = parent; }

QJsonObject WPNNode::attributesJson() const
{
    QJsonObject obj;

    obj.insert("FULL_PATH", m_attributes.path);
    obj.insert("TYPE", typeTag());
    obj.insert("ACCESS", static_cast<quint8>(m_attributes.access));
    obj.insert("VALUE", jsonValue());
    obj.insert("DESCRIPTION", m_attributes.description);
    obj.insert("CRITICAL", m_attributes.critical);

    if ( !m_attributes.extended_type.isEmpty() )
        obj.insert("EXTENDED_TYPE", m_attributes.extended_type );

    //obj.insert("TAGS", m_attributes.tags);
    //obj.insert("RANGE");
    // + clipmode todo

    return obj;
}

QJsonObject WPNNode::attributeJson(QString attr) const
{
    QJsonObject obj;

    if ( attr == "VALUE" ) obj.insert(attr, jsonValue());
    return obj;
}

QString WPNNode::typeTag() const
{
    switch ( m_attributes.type )
    {
    case Type::Values::Bool:         return "T";
    case Type::Values::Char:         return "c";
    case Type::Values::Float:        return "f";
    case Type::Values::Impulse:      return "N";
    case Type::Values::Int:          return "i";
    case Type::Values::List:         return "";
    case Type::Values::None:         return "null";
    case Type::Values::String:       return "s";
    case Type::Values::Vec2f:        return "ff";
    case Type::Values::Vec3f:        return "fff";
    case Type::Values::Vec4f:        return "ffff";
    }
}

QJsonValue WPNNode::jsonValue() const
{
    QJsonValue v;
    switch ( m_attributes.type )
    {
    case Type::Values::Bool:         v = m_attributes.value.toBool(); break;
    case Type::Values::Char:         v = m_attributes.value.toString(); break;
    case Type::Values::Float:        v = m_attributes.value.toDouble(); break;
    case Type::Values::String:       v = m_attributes.value.toString(); break;
    case Type::Values::Int:          v = m_attributes.value.toInt(); break;
    case Type::Values::None:         v = "null"; break;
    case Type::Values::Impulse:      return v;

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
    emit valueReceived(value);

    if ( m_attributes.value != value )
    {
        m_attributes.value = value;
        emit valueChanged(value);
    }
}

void WPNNode::setValue(QVariant value)
{
    m_attributes.value = value;

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

void WPNNode::setListening(bool listen, WPNDevice *target)
{
    if ( listen && !m_listeners.contains(target) )
        m_listeners.push_back(target);

    else m_listeners.removeAll(target);
}

void WPNNode::addSubnode(WPNNode *node)
{
    node->setParent(this);
    m_children.push_back(node);
}

WPNNode* WPNNode::createSubnode(QString name)
{
    auto node = new WPNNode;

    node->setPath       ( path()+"/"+name );
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

WPNNode* WPNNode::subnode(uint64_t index)
{
    return m_children[index];
}

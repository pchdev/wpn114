#include "node.hpp"
#include <QtDebug>

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

    m_name = m_attributes.path;
    m_name.remove(0, 1);

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

    //obj.insert("TAGS", m_attributes.tags);
    //obj.insert("RANGE");
    // + clipmode todo

    return obj;

}

QJsonObject WPNNode::attributeJson(QString attr) const
{
    QJsonObject obj;

    if ( attr == "VALUE" ) obj.insert("VALUE", jsonValue());
    return obj;
}

QString WPNNode::typeTag() const
{
    switch ( m_attributes.type )
    {
    case Type::Bool:         return "T";
    case Type::Char:         return "c";
    case Type::Float:        return "f";
    case Type::Impulse:      return "N";
    case Type::Int:          return "i";
    case Type::List:         return "b";
    case Type::None:         return "null";
    case Type::String:       return "s";
    case Type::Vec2f:        return "ff";
    case Type::Vec3f:        return "fff";
    case Type::Vec4f:        return "ffff";
    }
}

QJsonValue WPNNode::jsonValue() const
{
    QJsonValue v;
    switch ( m_attributes.type )
    {
    case Type::Bool:         v = m_attributes.value.toBool(); break;
    case Type::Char:         v = m_attributes.value.toString(); break;
    case Type::Float:        v = m_attributes.value.toDouble(); break;
    case Type::String:       v = m_attributes.value.toString(); break;
    case Type::Int:          v = m_attributes.value.toInt(); break;
    case Type::None:         v = "null"; break;
    case Type::Impulse:      return v;

//    case WPNNode::Type::List:       v = m_value.toList(); break;
//    case WPNNode::Type::Vec2f:      v = m_value.toList(); break;
//    case WPNNode::Type::Vec3f:      v = m_value.toList(); break;
//    case WPNNode::Type::Vec4f:      v = m_value.toList(); break;
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
    qDebug() << m_attributes.path
             << typeTag() << m_attributes.value;

    for ( const auto& child : m_children )
        child->post();
}

void WPNNode::setPath(QString path)
{
    m_attributes.path = path;
}

void WPNNode::setValueQuiet(QVariant value)
{
    emit valueReceived();

    if ( m_attributes.value != value )
    {
        m_attributes.value = value;
        emit valueChanged(value);
    }
}

void WPNNode::setValue(QVariant value)
{
    qDebug() << "new value:" << value;
    m_attributes.value = value;

    for ( const auto& listener : m_listeners )
         listener->pushNodeValue(this);
}

void WPNNode::setType(Type::Values type)
{
    m_attributes.type = type;
}


void WPNNode::setListening(bool listen, WPNDevice *target)
{
    qDebug() << "new listener";

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
        if ( child->subnode(path) ) return child;

    if ( path == m_attributes.path ) return this;

    return nullptr;
}

WPNNode* WPNNode::subnode(uint64_t index)
{
    return m_children[index];
}

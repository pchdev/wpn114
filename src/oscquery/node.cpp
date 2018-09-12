#include "node.hpp"
#include <QtDebug>

WPNNode::WPNNode() :
    m_device(nullptr), m_parent(nullptr), m_critical(false), m_type(WPNNode::Type::None)
{ }

WPNNode::~WPNNode() { }
void WPNNode::componentComplete()
{
    if ( m_path.isEmpty() )
        m_path = "/"+ m_name;

    m_name = m_path;
    m_name.remove(0, 1);

    if      ( m_device ) m_device->addNode(m_device, this);
    else if ( m_parent )
    {
        m_device = m_parent->device();
        m_device->addNode(m_device, this);
    }
}

QJsonObject WPNNode::attribute(QString attr) const
{
    QJsonObject obj;

    if ( attr == "VALUE" ) obj.insert("VALUE", valueJson());
    return obj;
}

QString WPNNode::typeString() const
{
    switch ( m_type )
    {
    case WPNNode::Type::Bool:         return "T";
    case WPNNode::Type::Char:         return "c";
    case WPNNode::Type::Float:        return "f";
    case WPNNode::Type::Impulse:      return "N";
    case WPNNode::Type::Int:          return "i";
    case WPNNode::Type::List:         return "b";
    case WPNNode::Type::None:         return "null";
    case WPNNode::Type::String:       return "s";
    case WPNNode::Type::Vec2f:        return "ff";
    case WPNNode::Type::Vec3f:        return "fff";
    case WPNNode::Type::Vec4f:        return "ffff";
    }
}

QJsonValue WPNNode::valueJson() const
{
    QJsonValue v;
    switch ( m_type )
    {
    case WPNNode::Type::Bool:         v = m_value.toBool(); break;
    case WPNNode::Type::Char:         v = m_value.toString(); break;
    case WPNNode::Type::Float:        v = m_value.toDouble(); break;
    case WPNNode::Type::Impulse:      return v;
    case WPNNode::Type::Int:          v = m_value.toInt(); break;
  //  case WPNNode::Type::List:       v = m_value.toList(); break;
    case WPNNode::Type::None:         v = "null"; break;
    case WPNNode::Type::String:       v = m_value.toString(); break;
//    case WPNNode::Type::Vec2f:      v = m_value.toList(); break;
//    case WPNNode::Type::Vec3f:      v = m_value.toList(); break;
//    case WPNNode::Type::Vec4f:      v = m_value.toList(); break;
    }

    return v;
}

QJsonObject WPNNode::info() const
{
    QJsonObject info;
    info.insert("DESCRIPTION", "No description available");
    info.insert("FULL_PATH", m_path);
    info.insert("ACCESS", m_type == WPNNode::Type::None ? 0 : 3 );
    info.insert("TYPE", typeString());
    info.insert("VALUE", valueJson());

    if ( m_children.empty() ) return info;

    QJsonObject contents;

    for ( const auto& child : m_children )
        contents.insert(child->name(), child->info());

    info.insert("CONTENTS", contents);
    return info;

}

void WPNNode::post() const
{
    qDebug() << m_path << m_type << m_value;

    for ( const auto& child : m_children )
        child->post();
}

void WPNNode::setName(QString name)
{
    m_name = name;
    emit nameChanged(name);
}

void WPNNode::setPath(QString path)
{
    m_path = path;
}

void WPNNode::setValueQuiet(QVariant value)
{
    emit valueReceived();

    if ( m_value != value )
    {
        m_value = value;
        emit valueChanged(value);
    }
}

void WPNNode::setValue(QVariant value)
{
    m_value = value;

    for ( const auto& listener : m_listeners )
         listener->pushNodeValue(this);

}

void WPNNode::setListening(bool listen, WPNDevice *target)
{
    if ( listen && !m_listeners.contains(target) )
        m_listeners.push_back(target);

    else m_listeners.removeAll(target);
}

void WPNNode::setCritical(bool critical)
{
    m_critical = critical;
}

void WPNNode::setDevice(WPNDevice* device)
{
    m_device = device;
}

void WPNNode::setType(WPNNode::Type type)
{
    m_type = type;
}

void WPNNode::setParent(WPNNode* parent)
{
    m_parent = parent;
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

    if ( path == m_path ) return this;

    return nullptr;
}

WPNNode* WPNNode::subnode(uint64_t index)
{
    return m_children[index];
}

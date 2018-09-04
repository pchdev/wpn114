#include "node.hpp"

QueryNode::QueryNode() :
    m_device(0), m_parent(0), m_critical(false), m_type(QueryNode::Type::None)
{ }

QueryNode::~QueryNode()
{

}

void QueryNode::classBegin() {}
void QueryNode::componentComplete()
{
    if ( m_path.isEmpty() )
        m_path = "/"+ m_name;

    m_name = m_path;
    m_name.remove(0, 1);

    if ( m_device ) m_device->findOrCreateNode(m_path);
    else if ( m_parent )
    {
        m_device = m_parent->device();
        m_device->findOrCreateNode(m_path);
    }
}

QString QueryNode::typeString() const
{
    switch ( m_type )
    {
    case QueryNode::Type::Bool:         return "T"; break;
    case QueryNode::Type::Char:         return "c"; break;
    case QueryNode::Type::Float:        return "f"; break;
    case QueryNode::Type::Impulse:      return "N"; break;
    case QueryNode::Type::Int:          return "i"; break;
    case QueryNode::Type::List:         return "b"; break;
    case QueryNode::Type::None:         return "null"; break;
    case QueryNode::Type::String:       return "s"; break;
    case QueryNode::Type::Vec2f:        return "ff"; break;
    case QueryNode::Type::Vec3f:        return "fff"; break;
    case QueryNode::Type::Vec4f:        return "ffff"; break;
    }
}

QJsonValue QueryNode::valueJson() const
{
    QJsonValue v;
    switch(m_type)
    {
    case QueryNode::Type::Bool:         v = m_value.toBool(); break;
    case QueryNode::Type::Char:         v = m_value.toString(); break;
    case QueryNode::Type::Float:        v = m_value.toFloat(); break;
    case QueryNode::Type::Impulse:      return v;
    case QueryNode::Type::Int:          v = m_value.toInt(); break;
  //  case QueryNode::Type::List:       v = m_value.toList(); break;
    case QueryNode::Type::None:         return v;
    case QueryNode::Type::String:       v = m_value.toString(); break;
//    case QueryNode::Type::Vec2f:      v = m_value.toList(); break;
//    case QueryNode::Type::Vec3f:      v = m_value.toList(); break;
//    case QueryNode::Type::Vec4f:      v = m_value.toList(); break;
    }

    return v;
}

QJsonObject QueryNode::info() const
{
    QJsonObject info;
    info.insert("DESCRIPTION", "No description available");
    info.insert("FULL_PATH", m_path);
    info.insert("ACCESS", 3);
    info.insert("TYPE", typeString());
    info.insert("VALUE", valueJson());

    if ( m_children.empty() ) return info;

    QJsonObject contents;

    for ( const auto& child : m_children )
        contents.insert(child->name(), child->info());

    info.insert("CONTENTS", contents);
    return info;

}

void QueryNode::post() const
{
    qDebug() << m_path << m_type << m_value;

    for ( const auto& child : m_children )
        child->post();
}

void QueryNode::setName(QString name)
{
    m_name = name;
    emit nameChanged(name);
}

void QueryNode::setPath(QString path)
{
    m_path   = path;
}

void QueryNode::setValueQuiet(QVariant value)
{
    emit valueReceived();

    if ( m_value != value )
    {
        m_value = value;
        emit valueChanged(value);
    }
}

void QueryNode::setValue(QVariant value)
{
    // TODO: check type

    m_value = value;

    if ( m_critical && m_device )
    {
        // TODO: if variant is list
        m_device->sendMessageWS(m_path, QVariantList { value });
    }

    else if ( !m_critical && m_device )
    {
        // TODO: if variant is list
        m_device->sendMessageUDP(m_path, QVariantList { value });
    }
}

void QueryNode::setCritical(bool critical)
{
    m_critical = critical;
}

void QueryNode::setDevice(OSCQueryDevice* device)
{
    m_device = device;
}

void QueryNode::setType(QueryNode::Type type)
{
    m_type = type;
}

QueryNode* QueryNode::createSubnode(QString name)
{
    auto node = new QueryNode;

    node->setPath       ( path()+"/"+name );
    node->setName       ( name );
    node->setParent     ( this );

    m_children.push_back ( node );

    return node;
}

void QueryNode::removeSubnode(QueryNode *node)
{
    m_children.removeAll(node);
}

void QueryNode::removeSubnode(QString name)
{

}

QueryNode* QueryNode::subnode(QString path)
{
    for ( const auto& child : m_children )
        if ( child->path() == path )
            return child;

    for ( const auto& child : m_children )
        if ( child->subnode(path) ) return child;

    return 0;
}

QueryNode* QueryNode::subnode(uint64_t index)
{
    return m_children[index];
}

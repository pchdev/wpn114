#include "oscqueryhandler.hpp"


//-------------------------------------------------------------------------------------------------------
// PARAMETER
//-------------------------------------------------------------------------------------------------------

QueryNode::QueryNode() : m_device(0), m_critical(false)
{

}

QueryNode::~QueryNode()
{
    if ( m_device ) m_device->removeNode(this);
}

void QueryNode::classBegin() {}

void QueryNode::componentComplete()
{
}

QString QueryNode::typeString() const
{
    switch ( m_type )
    {
    case QueryNode::Type::Bool: return "T"; break;
    case QueryNode::Type::Char: return "c"; break;
    case QueryNode::Type::Float: return "f"; break;
    case QueryNode::Type::Impulse: return "N"; break;
    case QueryNode::Type::Int: return "i"; break;
    case QueryNode::Type::List: return "b"; break;
    case QueryNode::Type::None: return "null"; break;
    case QueryNode::Type::String: return "s"; break;
    case QueryNode::Type::Vec2f: return "ff"; break;
    case QueryNode::Type::Vec3f: return "fff"; break;
    case QueryNode::Type::Vec4f: return "ffff"; break;
    }
}

QJsonValue QueryNode::valueJson() const
{
    QJsonValue v;
    switch(m_type)
    {
    case QueryNode::Type::Bool: v = m_value.toBool(); break;
    case QueryNode::Type::Char: v = m_value.toString(); break;
    case QueryNode::Type::Float: v = m_value.toFloat(); break;
    case QueryNode::Type::Impulse: return v;
    case QueryNode::Type::Int: v = m_value.toInt(); break;
  //  case QueryNode::Type::List: v = m_value.toList(); break;
    case QueryNode::Type::None: return v;
    case QueryNode::Type::String: v = m_value.toString(); break;
//    case QueryNode::Type::Vec2f: v = m_value.toList(); break;
//    case QueryNode::Type::Vec3f: v = m_value.toList(); break;
//    case QueryNode::Type::Vec4f: v = m_value.toList(); break;
    }

    return v;
}

QJsonObject QueryNode::info() const
{
    QJsonObject info;
    info.insert("DESCRIPTION", "No description available");
    info.insert("FULL_PATH", m_address);
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

void QueryNode::setName(QString name)
{
    m_name = name;
    emit nameChanged(name);
}

void QueryNode::setAddress(QString address)
{
    m_address = address;
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
        m_device->sendMessageWS(m_address, QVariantList { value });
    }

    else if ( !m_critical && m_device )
    {
        // TODO: if variant is list
        m_device->sendMessageUDP(m_address, QVariantList { value });
    }
}

void QueryNode::setCritical(bool critical)
{
    m_critical = critical;
}

void QueryNode::setDevice(OSCQueryDevice* device)
{
    m_device = device;
    device->addNode(this);
}

void QueryNode::setType(QueryNode::Type type)
{
    m_type = type;
}

void QueryNode::addChild(QueryNode *node)
{
    m_children.push_back(node);
}

void QueryNode::addChild(QString name)
{
    auto node = new QueryNode;
    node->setName(name);
    node->setAddress(address()+"/"+name);

    m_children.push_back(node);
}

void QueryNode::addChild(QStringList list)
{
    if ( list.size() == 1 )
    {
        addChild(list[0]);
        return;
    }

    auto next = getChild(list[0]);
    list.removeFirst();

    next->addChild(list);
}

void QueryNode::removeChild(QueryNode *node)
{
    m_children.removeAll(node);
}

QueryNode* QueryNode::getChild(QStringList list)
{


}

QueryNode* QueryNode::getChild(QString target)
{
    for ( const auto& child : m_children )
        if ( child->address() == target )
            return child;

    for ( const auto& child : m_children )
        if ( child->getChild(target) ) return child;

    return 0;
}

QueryNode* QueryNode::getChild(uint64_t index)
{
    return m_children[index];
}

QVector<QueryNode*> QueryNode::getChildren() const
{
    return m_children;
}

//-------------------------------------------------------------------------------------------------------
// DEVICE
//-------------------------------------------------------------------------------------------------------

OSCQueryDevice::OSCQueryDevice()
{
    m_osc_hdl   = new OSCHandler();
    m_root_node = new QueryNode;
    m_root_node->setAddress("/");

}

OSCQueryDevice::~OSCQueryDevice()
{

}

void OSCQueryDevice::addNode(QueryNode *node)
{
    auto address = node->address();
    auto spl = address.split('/');

    m_root_node->addChild(spl);
}

void OSCQueryDevice::removeNode(QueryNode *node)
{

}

void OSCQueryDevice::sendMessageUDP(QString address, QVariantList arguments)
{
    m_osc_hdl->sendMessage(address, arguments);
}

uint16_t OSCQueryDevice::oscPort() const
{
    return m_osc_hdl->localPort();
}

void OSCQueryDevice::setOscPort(uint16_t port)
{
    m_osc_hdl->setLocalPort(port);
}

QString OSCQueryDevice::deviceName() const
{
    return m_name;
}

void OSCQueryDevice::setDeviceName(QString name)
{
    m_name = name;
}

QueryNode* OSCQueryDevice::getRootNode()
{
    return m_root_node;
}

QueryNode* OSCQueryDevice::getNode(QString address)
{
    return m_root_node->getChild(address);
}

//-------------------------------------------------------------------------------------------------------
// SERVER
//-------------------------------------------------------------------------------------------------------

OSCQueryServer::OSCQueryServer() : m_ws_port(5986)
{
    m_name      = "WPN-SERVER";
    m_ws_hdl    = new QWebSocketServer(m_name, QWebSocketServer::NonSecureMode, this);

    QObject::connect(m_osc_hdl, SIGNAL(messageReceived(QString,QVariantList)), this, SIGNAL(messageReceived(QString,QVariantList)));
    QObject::connect(m_ws_hdl, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

    m_ws_hdl->listen(QHostAddress::Any, m_ws_port);
}

void OSCQueryServer::setWsPort(uint16_t port)
{
    m_ws_port = port;

    if ( m_ws_hdl->isListening() ) m_ws_hdl->close();
    m_ws_hdl->listen(QHostAddress::Any, m_ws_port);
}

void OSCQueryServer::sendMessageWS(QString address, QVariantList arguments)
{
    // json stringify

    qDebug() << address << arguments;
}

void OSCQueryServer::onNewConnection()
{
    qDebug() << "new connection";
    auto connection = m_ws_hdl->nextPendingConnection();
    QObject::connect(connection, SIGNAL(textMessageReceived(QString)), this, SLOT(onWSMessage(QString)));
    QObject::connect(connection, SIGNAL(disconnected()), this, SLOT(onDisconnection()));

    m_clients.push_back(connection);
    emit connected(connection->localAddress().toString());
    exposeHostInfo(connection);
    exposeHostTree(connection);
}

void OSCQueryServer::exposeHostInfo(QWebSocket *remote)
{

    QJsonObject reply;
    reply.insert("NAME", m_name);
    reply.insert("OSC_PORT", m_osc_hdl->localPort());
    reply.insert("OSC_TRANSPORT", "UDP");

    QJsonObject ext;
    ext.insert("TYPE", true);
    ext.insert("ACCESS", true);
    ext.insert("VALUE", true);
    ext.insert("RANGE", true);
    ext.insert("TAGS", true);
    ext.insert("CLIPMODE", true);
    ext.insert("UNIT", true);
    ext.insert("CRITICAL", true);
    ext.insert("HTML", true);
    ext.insert("OSC_STREAMING", true);
    ext.insert("LISTEN", true);
    ext.insert("PATH_CHANGED", true);
    ext.insert("PATH_RENAMED", true);
    ext.insert("PATH_ADDED", true);
    ext.insert("PATH_REMOVED", true);

    reply.insert("EXTENSIONS", ext);

    QJsonDocument r(reply);
    remote->sendTextMessage(r.toJson(QJsonDocument::Compact));
}

void OSCQueryServer::exposeHostTree(QWebSocket *remote)
{
    auto cdn = m_root_node->getChildren();
    QJsonObject rn, contents;

    rn.insert("FULL_PATH", "/");

    for ( const auto& child : cdn )
        contents.insert(child->name(), child->info());

    rn.insert("CONTENTS", contents);
    QJsonDocument tree(rn);

    remote->sendTextMessage(tree.toJson(QJsonDocument::Compact));
}

void OSCQueryServer::onDisconnection()
{
    QWebSocket* snd = qobject_cast<QWebSocket*>(sender());
    emit disconnected(snd->localAddress().toString());

    m_clients.removeAll(snd);
}

void OSCQueryServer::onWSMessage(QString msg)
{
    qDebug() << msg;

}

//-------------------------------------------------------------------------------------------------------
// CLIENT
//-------------------------------------------------------------------------------------------------------

OSCQueryClient::OSCQueryClient() : m_ws_hdl(new QWebSocket)
{
    QObject::connect(m_osc_hdl, SIGNAL(messageReceived(QString,QVariantList)), this, SIGNAL(messageReceived(QString,QVariantList)));
    QObject::connect(m_ws_hdl, SIGNAL(textMessageReceived(QString)), this, SLOT(onWSMessage(QString)));
    QObject::connect(m_ws_hdl, SIGNAL(connected()), this, SLOT(onNewConnection()));
    QObject::connect(m_ws_hdl, SIGNAL(disconnected()), this, SLOT(onDisconnection()));
}

void OSCQueryClient::setHostAddr(QString addr)
{
    m_host_addr = addr;
    m_ws_hdl->open(QUrl(addr));
}

void OSCQueryClient::sendMessageWS(QString address, QVariantList arguments)
{
    m_ws_hdl->sendTextMessage(address);
}

void OSCQueryClient::onWSMessage(QString msg)
{
    qDebug() << msg;
    auto jsobj = QJsonDocument::fromJson(msg.toUtf8()).object();

    for ( const auto& key : jsobj.keys() )
    {
        if ( key == "OSC_PORT" )
        {
            m_osc_hdl->setRemotePort(jsobj[key].toInt());
            QString reply("/?SET_PORT=");
            reply += QString::number(m_osc_hdl->remotePort());
            reply += "&LOCAL_PORT=";
            reply += QString::number(m_osc_hdl->localPort());

            m_ws_hdl->sendTextMessage("/");
            m_ws_hdl->sendTextMessage(reply);
        }
    }
}

void OSCQueryClient::onNewConnection()
{
    emit connected(m_ws_hdl->peerAddress().toString());
}

void OSCQueryClient::onDisconnection()
{
    emit disconnected(m_ws_hdl->peerAddress().toString());
}

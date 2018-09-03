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
    QObject::connect(m_osc_hdl, SIGNAL(messageReceived(QString,QVariantList)), this, SIGNAL(messageReceived(QString,QVariantList)));
    QObject::connect(m_ws_hdl, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

void OSCQueryServer::setWsPort(uint16_t port)
{
    m_ws_port = port;
    m_ws_hdl = new QWebSocketServer(m_name, QWebSocketServer::NonSecureMode, this);
}

void OSCQueryServer::sendMessageWS(QString address, QVariantList arguments)
{
    // json stringify

    qDebug() << address << arguments;
}

void OSCQueryServer::onNewConnection()
{
    auto connection = m_ws_hdl->nextPendingConnection();
    QObject::connect(connection, SIGNAL(textMessageReceived(QString)), this, SLOT(onWSMessage(QString)));
    QObject::connect(connection, SIGNAL(disconnected()), this, SLOT(onDisconnection()));

    m_clients.push_back(connection);
    emit connected(connection->localAddress().toString());
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
}

void OSCQueryClient::onNewConnection()
{
    emit connected(m_ws_hdl->peerAddress().toString());
}

void OSCQueryClient::onDisconnection()
{
    emit disconnected(m_ws_hdl->peerAddress().toString());
}

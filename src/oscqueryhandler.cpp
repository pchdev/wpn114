#include "oscqueryhandler.hpp"

//-------------------------------------------------------------------------------------------------------
// PARAMETER
//-------------------------------------------------------------------------------------------------------

QueryParameter::QueryParameter() : m_device(0), m_critical(false)
{

}

QueryParameter::~QueryParameter()
{
    if ( m_device ) m_device->unRegisterParameter(this);
}

void QueryParameter::classBegin() {}

void QueryParameter::componentComplete()
{

}

void QueryParameter::setAddress(QString address)
{
    m_address = address;
}

void QueryParameter::setValueFromNetwork(QVariant value)
{
    emit valueReceived();

    if ( m_value != value )
    {
        m_value = value;
        emit valueChanged(value);
    }
}

void QueryParameter::setValue(QVariant value)
{
    if ( value.type() != m_type ) return;

    m_value = value;

    if ( m_critical && m_device )
    {
        // TODO: if variant is list
        m_device->sendMessageWS(m_address, QVariantList{value} );
    }

    else if ( !m_critical && m_device )
    {
        // TODO: if variant is list
        m_device->sendMessageUDP(m_address, QVariantList{value});
    }
}

void QueryParameter::setCritical(bool critical)
{
    m_critical = critical;
}

void QueryParameter::setDevice(OSCQueryDevice* device)
{
    m_device = device;
    device->registerParameter(this);
}

void QueryParameter::setType(QMetaType::Type type)
{
    m_type = type;
}

//-------------------------------------------------------------------------------------------------------
// DEVICE
//-------------------------------------------------------------------------------------------------------

void OSCQueryDevice::registerParameter(QueryParameter* parameter)
{
    if ( !m_parameters.contains(parameter) )
        m_parameters.push_back(parameter);
}

void OSCQueryDevice::unRegisterParameter(QueryParameter* parameter)
{
    m_parameters.removeAll(parameter);
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
}

void OSCQueryServer::onNewConnection()
{
    auto connection = m_ws_hdl->nextPendingConnection();
    QObject::connect(connection, SIGNAL(textMessageReceived(QString)), this, SLOT(onWSMessage(QString)));
    QObject::connect(connection, SIGNAL(disconnected()), this, SLOT(onDisconnection()));

    m_clients.push_back(connection);
    emit clientConnected(connection->localAddress().toString());
}

void OSCQueryServer::onDisconnection()
{
    QWebSocket* snd = qobject_cast<QWebSocket*>(sender());
    emit clientDisconnected(snd->localAddress().toString());

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
    emit clientConnected(m_ws_hdl->peerAddress().toString());
}

void OSCQueryClient::onDisconnection()
{
    emit clientDisconnected(m_ws_hdl->peerAddress().toString());
}

QueryParameter* OSCQueryClient::parameter(QString address)
{
    for ( const auto& parameter : m_parameters )
        if ( parameter->address() == address )
            return parameter;
}




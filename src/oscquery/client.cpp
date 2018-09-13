#include "client.hpp"
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QCryptographicHash>
#include <QDataStream>
#include <src/http/http.hpp>

WPNQueryClient::WPNQueryClient() : WPNDevice(), m_osc_hdl(new OSCHandler())
{
    // direct client
    m_ws_con = new WPNWebSocket("127.0.0.1", 5678);
    QObject::connect(m_ws_con, SIGNAL(connected()), this, SIGNAL(connected()));
    QObject::connect(m_ws_con, SIGNAL(connected()), this, SLOT(onConnected()));
    QObject::connect(m_ws_con, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    QObject::connect(m_ws_con, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextMessageReceived(QString)));
}

WPNQueryClient::WPNQueryClient(WPNWebSocket* con) : m_osc_hdl(new OSCHandler())
{
    // indirect client (server image)
    // no need for a local udp port
    m_ws_con = con;
    QObject::connect(m_ws_con, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextMessageReceived(QString)));
    QObject::connect(m_ws_con, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
}

void WPNQueryClient::componentComplete()
{
    // if direct client: reach host
    m_ws_con->connect();
}

void WPNQueryClient::onConnected()
{
    // request host info
    // request namespace

    m_ws_con->write("/?HOST_INFO");
    m_ws_con->write("/");

    //m_ws_con->request   ( HTTP::formatRequest("/", "HOST_INFO", m_host_addr) );
    //m_ws_con->request   ( HTTP::formatRequest("/", "", m_host_addr) );
}

void WPNQueryClient::setHostAddr(QString addr)
{
    m_host_addr = addr;
    m_osc_hdl->setRemoteAddress(addr);
}

void WPNQueryClient::setPort(quint16 port)
{
    m_host_port = port;
}

void WPNQueryClient::setOscPort(quint16 port)
{
    m_osc_hdl->setRemotePort(port);
    //m_osc_hdl->listen();
}

void WPNQueryClient::onTextMessageReceived(QString message)
{
    // - host_info
    // - namespace
    // - value updates (criticals)
    // - ...
    // - signal chains...

    auto obj = QJsonDocument::fromJson(message.toUtf8()).object();

    if ( obj.contains("COMMAND")) emit command(obj);
    else if ( obj.contains("VALUE")) emit valueUpdate(obj);

    else if ( obj.contains("FULL_PATH")) onHostInfoReceived(obj);
    else if ( obj.contains("OSC_PORT")) onNamespaceReceived(obj);

    qDebug() << "WebSocket In:" << message;
}

void WPNQueryClient::onHostInfoReceived(QJsonObject info)
{

}

void WPNQueryClient::onNamespaceReceived(QJsonObject nspace)
{

}

void WPNQueryClient::writeOsc(QString method, QVariantList arguments)
{

}

void WPNQueryClient::writeWebSocket(QString method, QVariantList arguments)
{

}

void WPNQueryClient::writeWebSocket(QString message)
{
    m_ws_con->write(message);
}

void WPNQueryClient::writeWebSocket(QJsonObject json)
{
    m_ws_con->write(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

void WPNQueryClient::pushNodeValue(WPNNode* node)
{
    if ( node->critical() )
        m_ws_con->write(QJsonDocument(node->attributeJson("VALUE")).toJson(QJsonDocument::Compact));

    else m_osc_hdl->sendMessage(node->path(), QVariantList{node->value()});
}


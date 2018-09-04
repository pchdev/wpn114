#include "server.hpp"
#include <QJsonDocument>

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
    m_ws_hdl->listen ( QHostAddress::Any, m_ws_port );
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
    auto cdn = m_root_node->subnodes();
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

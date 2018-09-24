#include "query-server.hpp"
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCryptographicHash>
#include <QDateTime>
#include <QRegExp>
#include <QBitArray>
#include "../http/http.hpp"
#include "file.hpp"
#include <QNetworkReply>
#include <chrono>
#include <thread>


WPNQueryServer::WPNQueryServer() : WPNDevice (), m_ws_server(new WPNWebSocketServer(5678)),
    m_osc_hdl(new OSCHandler())
{
    // default settings & extensions

    m_settings.name             = "WPN-SERVER";
    m_settings.tcp_port         = 5678;
    m_settings.osc_port         = 1234;
    m_settings.osc_transport    = "UDP";

    m_settings.extensions.osc_streaming     = true;
    m_settings.extensions.access            = true;
    m_settings.extensions.clipmode          = true;
    m_settings.extensions.critical          = true;
    m_settings.extensions.description       = true;
    m_settings.extensions.extended_type     = true;
    m_settings.extensions.listen            = true;
    m_settings.extensions.path_added        = false;
    m_settings.extensions.path_changed      = false;
    m_settings.extensions.path_removed      = false;
    m_settings.extensions.path_renamed      = false;
    m_settings.extensions.range             = false;
    m_settings.extensions.tags              = true;
    m_settings.extensions.unit              = false;
    m_settings.extensions.value             = true;
}

void WPNQueryServer::componentComplete()
{
    QObject::connect( m_ws_server, SIGNAL(newConnection(WPNWebSocket*)),
                      this, SLOT(onNewConnection(WPNWebSocket*)));

    QObject::connect( m_ws_server, SIGNAL(httpRequestReceived(QTcpSocket*,QString)),
                      this, SLOT(onHttpRequest(QTcpSocket*,QString)));

    QObject::connect( m_osc_hdl, SIGNAL(messageReceived(QString,QVariant)),
                      this, SLOT(onValueUpdate(QString,QVariant)));

    m_osc_hdl->setLocalPort     ( m_settings.osc_port );
    m_ws_server->setPort        ( m_settings.tcp_port );

    m_ws_server->start          ( );
    m_osc_hdl->listen           ( );

    m_zeroconf.startServicePublish(
                m_settings.name.toStdString().c_str(), "_oscjson._tcp", "local", m_settings.tcp_port);
}

void WPNQueryServer::setTcpPort(quint16 port)
{
    m_settings.tcp_port     = port;
    m_ws_server->setPort    ( port );
}

void WPNQueryServer::setUdpPort(quint16 port)
{
    m_settings.osc_port     = port;
    m_osc_hdl->setLocalPort ( port );
}

void WPNQueryServer::onNewConnection(WPNWebSocket* con)
{
    auto client = new WPNQueryClient(con);
    m_clients.push_back(client);   

    QObject::connect ( client, SIGNAL(valueUpdate(QJsonObject)),
                      this, SLOT(onValueUpdate(QJsonObject)));

    QObject::connect ( client, SIGNAL(command(QJsonObject)),
                       this, SLOT(onCommand(QJsonObject)));

    QObject::connect ( client, SIGNAL(httpMessageReceived(QString)),
                       this, SLOT(onClientHttpQuery(QString)));

    qDebug() << "[OSCQUERY-SERVER] New client connection";
    emit newConnection();
}

void WPNQueryServer::onClientHttpQuery(QString query)
{
    WPNQueryClient* sender = qobject_cast<WPNQueryClient*>(QObject::sender());
    onHttpRequest(sender->tcpConnection(), query);
}

void WPNQueryServer::onHttpRequest(QTcpSocket* sender, QString req)
{
    auto spl = req.split("GET", QString::SkipEmptyParts);

    for ( const auto& request : spl )
    {
        if ( request.contains("HOST_INFO") )
        {
            HTTP::Reply rep;
            rep.target = sender;
            rep.reply = hostInfoJson().toUtf8();
            m_reply_manager.enqueue(rep);
        }
        else
        {
            HTTP::Reply rep;
            rep.target = sender;
            auto spl2 = request.split(' ', QString::SkipEmptyParts);
            rep.reply = namespaceJson(spl2[0]).toUtf8();
            m_reply_manager.enqueue(rep);
        }
    }
}

QString WPNQueryServer::hostInfoJson()
{
    qDebug() << "[OSCQUERY-SERVER] HOST_INFO requested";
    return HTTP::ReplyManager::formatJsonResponse(m_settings.toJson());
}

QString WPNQueryServer::namespaceJson(QString method)
{
    qDebug() << "[OSCQUERY-SERVER] NAMESPACE requested for method:" << method ;

    if ( method.contains("?") )
    {
        auto spl   = method.split('?');
        auto node  = m_root_node->subnode(spl[0]);

        if ( !node ) return "";

        auto obj   = node->attributeJson(spl[1]);
        return HTTP::ReplyManager::formatJsonResponse(obj);
    }
    else
    {
        WPNNode* node  = m_root_node->subnode(method);

        if ( !node )
        {
            emit unknownMethodRequested(method);
            return ""; // TODO: 404 reply
        }

        if ( auto file = dynamic_cast<WPNFileNode*>(node) )
        {
            // if node is a file
            // reply with the contents of the file

            if ( file->path().endsWith(".png"))
                return HTTP::ReplyManager::formatFileResponse(file->data(), "image/png");

            else if ( file->path().endsWith(".qml"))
                return HTTP::ReplyManager::formatFileResponse(file->data(), "text/plain");
        }

        return HTTP::ReplyManager::formatJsonResponse(node->toJson());
    }
}

void WPNQueryServer::onCommand(QJsonObject command_obj)
{
    WPNQueryClient* listener = qobject_cast<WPNQueryClient*>(QObject::sender());

    QString command = command_obj["COMMAND"].toString();

    if ( command == "LISTEN" || command == "IGNORE" )
    {
        QString method = command_obj["DATA"].toString();
        auto node = m_root_node->subnode(method);
        node->setListening(command == "LISTEN", listener);
    }
    else if ( command == "START_OSC_STREAMING" )
    {
        quint16 port = command_obj["DATA"].toObject()["LOCAL_SERVER_PORT"].toInt();
        listener->setOscPort(port);
    }
}

void WPNQueryServer::pushNodeValue(WPNNode* node)
{

}

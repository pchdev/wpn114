#include "query-server.hpp"
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCryptographicHash>
#include <QDateTime>
#include <QRegExp>
#include <QBitArray>
#include "../http/http.hpp"

QJsonObject HostExtensions::toJson() const
{
    QJsonObject ext
    {
        { "OSC_STREAMING", osc_streaming },
        { "ACCESS", access },
        { "VALUE", value },
        { "RANGE", range },
        { "DESCRIPTION", description },
        { "TAGS", tags },
        { "EXTENDED_TYPE", extended_type },
        { "UNIT", unit },
        { "CRITICAL", critical },
        { "CLIPMODE", clipmode },
        { "LISTEN", listen },
        { "PATH_CHANGED", path_changed },
        { "PATH_REMOVED", path_removed },
        { "PATH_ADDED", path_added },
        { "PATH_RENAMED", path_renamed }
    };

    return ext;
}

QJsonObject HostSettings::toJson() const
{
    QJsonObject obj
    {
        { "NAME", name },
        { "OSC_PORT", osc_port },
        { "OSC_TRANSPORT", osc_transport },
        { "EXTENSIONS", extensions.toJson() }
    };

    return obj;
}

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
    m_settings.extensions.extended_type     = false;
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

    QObject::connect( m_osc_hdl, SIGNAL(messageReceived(QString,QVariantList)),
                      this, SLOT(onValueUpdate(QString,QVariantList)));

    m_osc_hdl->setLocalPort     ( m_settings.osc_port );
    m_ws_server->setPort        ( m_settings.tcp_port );

    m_ws_server->start          ( );
    m_osc_hdl->listen           ( );
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
}

void WPNQueryServer::onHttpRequest(QTcpSocket* sender, QString req)
{
    if ( req.contains("HOST_INFO") ) onHostInfoRequest(sender);
    else if ( req.contains("GET /") )
    {
        auto spl = req.split(' ');
        onNamespaceRequest(sender, spl[1]);
    }
}

void WPNQueryServer::onHostInfoRequest(QTcpSocket* sender)
{
    auto resp = HTTP::formatJsonResponse(m_settings.toJson());
    if ( sender ) sender->write(resp.toUtf8());

    for ( const auto& client : m_clients )
        client->writeWebSocket  ( m_settings.toJson() );
}

void WPNQueryServer::onNamespaceRequest(QTcpSocket* sender, QString method)
{
    if ( method.contains("?") )
    {
        auto spl        = method.split('?');
        auto obj        = ( spl[1], m_root_node->subnode(spl[0])->attributeJson(spl[1]) );
        auto resp       = HTTP::formatJsonResponse(obj);
        sender->write   ( resp.toUtf8() );

        for ( const auto& client : m_clients )
            client->writeWebSocket(obj);
    }
    else
    {
        WPNNode* node       = m_root_node->subnode(method);
        auto resp           = HTTP::formatJsonResponse(node->toJson());
        sender->write       ( resp.toUtf8() );

        for ( const auto& client : m_clients )
            client->writeWebSocket ( m_root_node->subnode(method)->toJson() );
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

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
    if ( !m_host_addr.isEmpty()) m_ws_con->connect();
    else if ( !m_zconf_host.isEmpty() )
    {
        m_zconf.startBrowser("_oscjson._tcp");
        QObject::connect( &m_zconf, SIGNAL(serviceAdded(QZeroConfService)),
                          this, SLOT(onZeroConfServiceAdded(QZeroConfService)) );
    }
}

void WPNQueryClient::onConnected()
{
    // request host info
    // request namespace

    m_ws_con->write("/?HOST_INFO");
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

void WPNQueryClient::onZeroConfServiceAdded(QZeroConfService service)
{
    if ( service.name() == m_zconf_host )
    {
        m_ws_con->connect   ( service.ip().toString(), service.port() );
        m_zconf.stopBrowser ( );
    }
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

    else if ( obj.contains("OSC_PORT")) onHostInfoReceived(obj);
    else if ( obj.contains("FULL_PATH")) onNamespaceReceived(obj);

    qDebug() << "WebSocket In:" << message;
}

void WPNQueryClient::onHostInfoReceived(QJsonObject info)
{
    if ( info.contains("NAME" ) )  m_settings.name = info["NAME"].toString();
    if ( info.contains("OSC_PORT") ) m_settings.osc_port = info["OSC_PORT"].toInt();
    if ( info.contains("OSC_TRANSPORT")) m_settings.osc_transport = info["OSC_TRANSPORT"].toString();

    QJsonObject ext;
    if ( info.contains("EXTENSIONS") ) ext = info["EXTENSIONS"].toObject();
    else return;

    if ( ext.contains("ACCESS")) m_settings.extensions.access = ext["ACCESS"].toBool();
    if ( ext.contains("VALUE")) m_settings.extensions.value = ext["VALUE"].toBool();
    if ( ext.contains("RANGE")) m_settings.extensions.range = ext["RANGE"].toBool();
    if ( ext.contains("TAGS")) m_settings.extensions.tags = ext["TAGS"].toBool();
    if ( ext.contains("CLIPMODE")) m_settings.extensions.clipmode = ext["CLIPMODE"].toBool();
    if ( ext.contains("UNIT")) m_settings.extensions.unit = ext["UNIT"].toBool();
    if ( ext.contains("CRITICAL")) m_settings.extensions.critical = ext["CRITICAL"].toBool();
    if ( ext.contains("DESCRIPTION")) m_settings.extensions.description = ext["DESCRIPTION"].toBool();
    if ( ext.contains("OSC_STREAMING")) m_settings.extensions.osc_streaming = ext["OSC_STREAMING"].toBool();
    if ( ext.contains("LISTEN")) m_settings.extensions.listen = ext["LISTEN"].toBool();
    if ( ext.contains("PATH_CHANGED")) m_settings.extensions.path_changed = ext["PATH_CHANGED"].toBool();
    if ( ext.contains("PATH_RENAMED")) m_settings.extensions.path_renamed = ext["PATH_RENAMED"].toBool();
    if ( ext.contains("PATH_ADDED")) m_settings.extensions.path_added = ext["PATH_ADDED"].toBool();
    if ( ext.contains("PATH_REMOVED")) m_settings.extensions.path_removed = ext["PATH_REMOVED"].toBool();

    if ( m_settings.extensions.osc_streaming ) requestStreamStart();

    m_ws_con->write("/");
    m_ws_con->write("/play");

}

void WPNQueryClient::requestHttp(QString address)
{
    m_ws_con->request(HTTP::formatRequest(address, "", m_host_addr));
}

void WPNQueryClient::requestStreamStart()
{
    QJsonObject command, data;

    command.insert      ( "COMMAND", "START_OSC_STREAMING" );
    data.insert         ( "LOCAL_SERVER_PORT", m_osc_hdl->localPort() );
    data.insert         ( "LOCAL_SENDER_PORT", m_osc_hdl->remotePort() );
    command.insert      ( "DATA", data );

    m_ws_con->write     ( QJsonDocument(command).toJson(QJsonDocument::Compact) );
}

void WPNQueryClient::onNamespaceReceived(QJsonObject nspace)
{
    QJsonObject contents = nspace["CONTENTS"].toObject();

    for ( const auto& key : contents.keys() )
    {
        QJsonObject jsnode = contents[key].toObject();
        auto node = WPNNode::fromJson(jsnode);

        m_root_node->addSubnode(node);
    }
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


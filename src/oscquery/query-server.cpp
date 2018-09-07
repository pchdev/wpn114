#include "query-server.hpp"
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCryptographicHash>
#include <QDateTime>
#include <QRegExp>
#include <QBitArray>

QJsonObject HostExtensions::toJson() const
{
    QJsonObject ext
    {
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

QPair<Http::Request, QString> Http::parseRequest( QByteArray data )
{
    if ( data.contains("Sec-WebSocket-Key"))
    {
        // parse the key
        auto spl = data.split('\n');
        for ( const auto& line : spl )
        {
            if ( line.startsWith("Sec-WebSocket-Key"))
            {
                auto spl2   = line.split(' ');
                QString key = spl2[1];

                key.replace('\r',"");
                return QPair<Http::Request,QString>(Http::Request::HANDSHAKE, key.toUtf8());
            }
        }
        return QPair<Http::Request,QString>(Http::Request::UNKNOWN, QString());
    }
    else if ( data.contains("/?HOST_INFO") )
        return QPair<Http::Request,QString>(Http::Request::HOST_INFO, QString());

    else if ( data.contains("?") )
    {
        QString attr = data.split(' ')[1];
        return QPair<Http::Request,QString>(Http::Request::ATTRIBUTE, attr);
    }

    else if ( data.startsWith("GET"))
        return QPair<Http::Request,QString>(Http::Request::NAMESPACE, data.split(' ')[1]);

    return QPair<Http::Request,QString>(Http::Request::UNKNOWN, QString());
}

QString Http::formatResponse( QString response )
{
    QString resp    ( "HTTP/1.1 200 OK\r\n" );
    resp.append     ( QDateTime::currentDateTime().toString("ddd, dd MMMM yyyy hh:mm:ss t" ));
    resp.append     ( "\r\n" );
    resp.append     ( "Server: Qt/5.1.1\r\n" );
    resp.append     ( "Content-Type: application/json\r\n" );
    resp.append     ( "Content-Length: " );
    resp.append     ( QString::number(response.size()) );
    resp.append     ( "\r\n" );
    resp.append     ( "\r\n" );
    resp.append     ( response );

    return resp;
}

OSCQueryServer::OSCQueryServer() :
    m_tcp_server( new QTcpServer(this)), OSCQueryDevice ()
{
    // default settings & extensions

    m_settings.name             = "WPN-SERVER";
    m_settings.tcp_port         = 5678;
    m_settings.osc_port         = 1234;
    m_settings.osc_transport    = "UDP";

    m_settings.extensions.access            = false;
    m_settings.extensions.clipmode          = false;
    m_settings.extensions.critical          = false;
    m_settings.extensions.description       = false;
    m_settings.extensions.extended_type     = false;
    m_settings.extensions.listen            = false;
    m_settings.extensions.path_added        = false;
    m_settings.extensions.path_changed      = false;
    m_settings.extensions.path_removed      = false;
    m_settings.extensions.path_renamed      = false;
    m_settings.extensions.range             = false;
    m_settings.extensions.tags              = false;
    m_settings.extensions.unit              = false;
    m_settings.extensions.value             = false;

    QObject::connect(m_tcp_server, SIGNAL(newConnection()), this, SLOT(onNewTcpConnection()));
}

void OSCQueryServer::componentComplete()
{
    m_tcp_server->listen ( QHostAddress::Any, m_settings.tcp_port );
}

void OSCQueryServer::setUdpPort ( quint16 port )
{
    m_settings.osc_port = port;
    m_osc_hdl->setLocalPort(port);
}

void OSCQueryServer::onNewTcpConnection()
{
    while ( m_tcp_server->hasPendingConnections() )
    {
        QTcpSocket* con = m_tcp_server->nextPendingConnection();
        QObject::connect(con, SIGNAL(readyRead()), this, SLOT(onTcpDataReceived()));
    }
}

void OSCQueryServer::onTcpDataReceived()
{
    QTcpSocket* sender = qobject_cast<QTcpSocket*>(QObject::sender());

    while ( sender->bytesAvailable())
    {
        QByteArray data = sender->readAll();

        qDebug() << "TCP in:" << data;

        if ( data.contains("HTTP"))
        {
            auto req = Http::parseRequest(data);

            switch ( req.first )
            {
            case Http::Request::HANDSHAKE: onHandshakeRequest(req.second, sender); break;
            case Http::Request::HOST_INFO: onHostInfoQuery(sender); break;
            case Http::Request::NAMESPACE: onNamespaceQuery(req.second, sender); break;
            case Http::Request::COMMAND: onQueryCommand(req.second, sender); break;
            case Http::Request::ATTRIBUTE: onAttributeQuery(req.second, sender); break;
            case Http::Request::UNKNOWN: qDebug() << "Unknown request";
            }
        }
        else // it would be a websocket message
        {
            onWebsocketDataReceived(data);
        }
    }
}

void OSCQueryServer::onHandshakeRequest(QString key, QTcpSocket* sender)
{
    // concat key with 258EAFA5-E914-47DA-95CA-C5AB0DC85B11
    // take the SHA-1 of the result
    // return the base64 encoding of the hash

    key.append      ("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
    auto hash       = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Sha1);
    auto b64        = hash.toBase64();

    QString resp    ( "HTTP/1.1 101 Switching Protocols\r\n" );
    resp.append     ( "Upgrade: websocket\r\n" );
    resp.append     ( "Connection: Upgrade\r\n" );
    resp.append     ( "Sec-WebSocket-Accept: " );
    resp.append     ( b64 );
    resp.append     ( "\r\n");
    resp.append     ( "\r\n");

    sender->write ( resp.toUtf8() );
    m_ws_connections.push_back(sender);
}

void OSCQueryServer::onWebsocketDataReceived(QByteArray data)
{
    // decrypt encrypted message
    QDataStream stream(data);
    QByteArray decoded;

    quint8 fbyte, fin, opcode, sbyte, mask[4];
    quint64 payle;

    stream >> fbyte >> sbyte;

    if      ( fbyte-128 > 0 ) { fin = true; opcode = fbyte-128; }
    else    { fin = false; opcode = fbyte; }

    if      ( sbyte-128 <= 125 ) payle = sbyte-128;
    else if ( sbyte-128 == 127 ) stream >> payle;
    else if ( sbyte-128 == 126 ) { quint16 payle16; stream >> payle16; payle = payle16; }

    for ( quint8 i = 0; i < 4; ++i ) stream >> mask[i];
    for ( quint8 i = 0; i < payle; ++i )
    {
        quint8 byte;
        stream >> byte;
        decoded.append(byte^mask[i%4]);
    }

    qDebug() << "WebSocket in:" << decoded;
}

void OSCQueryServer::onNamespaceQuery(QString method, QTcpSocket* sender)
{
    QTcpSocket* con = getWebSocketConnectionFromSender(sender);
    QJsonObject info;

    if ( method == "/" ) info = m_root_node->info();
    else info = m_root_node->subnode(method)->info();

    write(con, QJsonDocument(info).toJson(QJsonDocument::Compact));
}

inline QTcpSocket* OSCQueryServer::getWebSocketConnectionFromSender(QTcpSocket* sender)
{
    for ( const auto& con : m_ws_connections )
        if ( con->peerAddress() == sender->peerAddress() )
            return con;

    return nullptr;
}

void OSCQueryServer::onHostInfoQuery(QTcpSocket* sender)
{
    QTcpSocket* con = getWebSocketConnectionFromSender(sender);
    write(con, QJsonDocument(m_settings.toJson()).toJson(QJsonDocument::Compact));
}

void OSCQueryServer::onQueryCommand( QString cmd, QTcpSocket* sender )
{

}

void OSCQueryServer::onAttributeQuery( QString query, QTcpSocket* sender )
{
    auto spl = query.split('?');

    QJsonObject obj = m_root_node->subnode(spl[0])->attribute(spl[1]);
    QTcpSocket* con = getWebSocketConnectionFromSender(sender);

    write(con, QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

void OSCQueryServer::writeWebSocket ( QString addr, QVariantList arguments )
{

}

void OSCQueryServer::write(QTcpSocket* target, QString message)
{
    QByteArray data;
    QDataStream stream ( &data, QIODevice::WriteOnly );

    stream << (quint8) 129;
    quint64 sz = message.size();

    // no need to set the mask bit here
    if ( sz > 65535 )
    {
        stream << (quint8) 127;
        stream << (quint64) message.size();
    }
    else if ( sz > 125 )
    {
        stream << (quint8) 126;
        stream << (quint16) message.size();
    }
    else stream << (quint8) message.size();

    data.append(message.toLatin1());
    target->write(data);

    qDebug() << "WebSocket Out:" << message;
}

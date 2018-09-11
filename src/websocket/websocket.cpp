#include "websocket.hpp"
#include <QDataStream>
#include <QRandomGenerator>
#include <QCryptographicHash>

// SERVER ----------------------------------------------------------------------------------------------

WPNWebSocketServer::WPNWebSocketServer(quint16 port) : m_port(port),
    m_tcp_server(new QTcpServer())
{
    QObject::connect(m_tcp_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

void WPNWebSocketServer::start()
{
    m_tcp_server->listen(QHostAddress::Any, m_port);
}

void WPNWebSocketServer::stop()
{
    m_tcp_server->close();
}

void WPNWebSocketServer::onNewConnection()
{
    // catching new tcp connection
    while ( m_tcp_server->hasPendingConnections() )
    {
        QTcpSocket* con = m_tcp_server->nextPendingConnection();
        QObject::connect(con, SIGNAL(readyRead()), this, SLOT(onTcpReadyRead()));
    }
}

void WPNWebSocketServer::onTcpReadyRead()
{
     QTcpSocket* sender = qobject_cast<QTcpSocket*>(QObject::sender());

     while ( sender->bytesAvailable())
     {
         QByteArray data = sender->readAll ( );
         if ( data.contains("Sec-WebSocket-Key"))
         {
             onHandshakeRequest(sender, data);
             // once handshake has been done, no need to keep it
             QObject::disconnect(sender, SIGNAL(readyRead()), this, SLOT(onTcpReadyRead()));
         }

         else if ( data.contains("HTTP") )
         {
             // parsing todo
         }
     }
}

void WPNWebSocketServer::onHandshakeRequest(QTcpSocket *sender, QByteArray data)
{
    // parse the key
    auto spl = data.split('\n');

    for ( const auto& line : spl )
    {
        if ( line.startsWith ( "Sec-WebSocket-Key" ))
        {
            auto spl2       = line.split(' ');
            QString key     = spl2[1];
            key.replace     ( "\r", "" );

            sendHandshakeResponse(sender, key);
        }
    }
}

void WPNWebSocketServer::sendHandshakeResponse ( QTcpSocket* target, QString key )
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

    // write accept response
    target->write ( resp.toUtf8() );

    // upgrade connection to websocket
    WPNWebSocket* client = new WPNWebSocket(target);
    emit newConnection(client);
}

void WPNWebSocketServer::onHttpRequestReceived(QString req)
{
    WPNWebSocket* sender = qobject_cast<WPNWebSocket*>(QObject::sender());
    emit httpRequestReceived(sender, req);
}

// CLIENT ----------------------------------------------------------------------------------------------

WPNWebSocket::WPNWebSocket(QString host_addr, quint16 port) :
    m_host_addr(host_addr), m_host_port(port)
{
    // direct client case
    QObject::connect(m_tcp_con, SIGNAL(connected()), this, SLOT(onConnected()));
    QObject::connect(m_tcp_con, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    QObject::connect(m_tcp_con, SIGNAL(readyRead()), this, SLOT(onRawMessageReceived()));
}

WPNWebSocket::WPNWebSocket(QTcpSocket* con) : m_tcp_con(con)
{
    // server catching a client
    // the proxy is already connected, so nothing to do here, except chain signals
    QObject::connect(m_tcp_con, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    QObject::connect(m_tcp_con, SIGNAL(readyRead()), this, SLOT(onRawMessageReceived()));
}

void WPNWebSocket::connect()
{
    m_tcp_con->connectToHost(m_host_addr, m_host_port);
}

void WPNWebSocket::disconnect()
{
    m_tcp_con->close();
}

void WPNWebSocket::onConnected()
{
    // tcp-connection established,
    // send http 'handshake' upgrade request to server
    generateEncryptedSecKey();
    requestHandshake();
}

void WPNWebSocket::onRawMessageReceived()
{
    QTcpSocket* sender = qobject_cast<QTcpSocket*>(QObject::sender());

    while ( sender->bytesAvailable())
        decode(sender->readAll());
}

QString WPNWebSocket::generateEncryptedSecKey()
{
    QRandomGenerator kgen;
    QByteArray res;

    for ( quint8 i = 0; i < 25; ++i )
    {
        quint8 rdm = kgen.generate64();
        res.append(rdm);
    }

    m_sec_key           = res.toBase64();
    QByteArray key      = m_sec_key;
    key.append          ( "258EAFA5-E914-47DA-95CA-C5AB0DC85B11" );
    auto hash           = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
    m_accept_key        = hash.toBase64();
}

void WPNWebSocket::requestHandshake()
{
    QByteArray req;
    req.append  ( "GET / HTTP/1.1\r\n" );
    req.append  ( "Connection: Upgrade\r\n" );
    req.append  ( m_host_addr.replace("ws://", "").append("\r\n").prepend("Host: "));
    req.append  ( "Sec-WebSocket-Key: " );
    req.append  ( m_sec_key.append("\r\n") );
    req.append  ( "Sec-WebSocket-Version: 13\r\n" );
    req.append  ( "Upgrade: websocket\r\n" );
    req.append  ( "User-Agent: Qt/5.1.1\r\n\r\n" );

    m_tcp_con->write(req);
}

void WPNWebSocket::write(QString message)
{
    QByteArray data;
    quint8 mask[4], size_mask;
    quint64 sz = message.size();

    QDataStream stream ( &data, QIODevice::WriteOnly );
    stream << (quint8) 129;

    // we have to set the mask bit here
    if ( sz > 65535 )
    {
        stream << (quint8) 255;
        stream << (quint64) message.size();
    }
    else if ( sz > 125 )
    {
        stream << (quint8) 254;
        stream << (quint16) message.size();
    }
    else stream << ( (quint8) message.size() + 128 );

    for ( quint8 i = 0; i < 4; ++i )
    {
        QRandomGenerator rdm;
        mask[i] = rdm.bounded(256);
        stream << mask[i];
    }

    // ...and encode the message with it
    for ( quint8 i = 0; i < message.size(); ++i )
        stream << ( mask[i%4] ^ message[i].toLatin1() );

    m_tcp_con->write(data);
    qDebug() << "WebSocket Out:" << message;
}

void WPNWebSocket::decode(QByteArray data)
{
    // coming from the server, it should be unmasked, but it might not be
    // decrypt encrypted message
    QDataStream stream(data);
    QByteArray decoded;

    quint8 fbyte, fin, opcode, sbyte, maskbit, mask[4];
    quint64 payle;

    stream >> fbyte >> sbyte;

    if      ( fbyte-128 > 0 ) { fin = true; opcode = fbyte-128; }
    else    { fin = false; opcode = fbyte; }

    if ( sbyte-128 <= 0 )
    {
        maskbit = 0;
        if ( sbyte <= 125 ) payle = sbyte;
        else if ( sbyte == 127 ) stream >> payle;
        else if ( sbyte == 126 ) {
            quint16 payle16;
            stream >> payle16;
            payle = payle16;
        }
        for ( quint64 i = 0; i < payle; ++i )
        {
            quint8 byte;
            stream >> byte;
            decoded.append(byte);
        }
    }
    else
    {
        maskbit = 1;

        if      ( sbyte-128 <= 125 ) payle = sbyte-128;
        else if ( sbyte-128 == 127 ) stream >> payle;
        else if ( sbyte-128 == 126 ) {
            quint16 payle16;
            stream >> payle16;
            payle = payle16;
        }

        for ( quint8 i = 0; i < 4; ++i ) stream >> mask[i];
        for ( quint64 i = 0; i < payle; ++i )
        {
            quint8 byte;
            stream >> byte;
            decoded.append ( byte^mask[i%4] );
        }
    }

    emit textMessageReceived(decoded);
}


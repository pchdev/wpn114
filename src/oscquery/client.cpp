#include "client.hpp"
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QCryptographicHash>

OSCQueryClient::OSCQueryClient() : OSCQueryDevice(), m_ws_socket(new QTcpSocket()),
    m_host_addr("127.0.0.1"), m_host_port(5678)
{
}

void OSCQueryClient::componentComplete()
{
    if ( m_host_addr.isEmpty() ) return;

    QString host = m_host_addr.replace("ws://", "");
    m_ws_socket->connectToHost(host, m_host_port);

    // when tcp connection is established, request http handshake
    QObject::connect(m_ws_socket, SIGNAL(connected()), this, SLOT(requestHandshake()));

}

void OSCQueryClient::generateSecKey()
{
    QRandomGenerator kgen;
    QByteArray res;

    for ( quint8 i = 0; i < 25; ++i )
    {
        quint8 rdm = kgen.generate64();
        res.append(rdm);
    }

    m_sec_ws_key = res.toBase64();

    QByteArray key      = m_sec_ws_key;
    key.append          ( "258EAFA5-E914-47DA-95CA-C5AB0DC85B11" );
    auto hash           = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
    m_sec_accept_key    = hash.toBase64();
}

void OSCQueryClient::requestHandshake()
{
    QByteArray req;
    generateSecKey();

    QObject::connect(m_ws_socket, SIGNAL(readyRead()), this, SLOT(onHandshakeResponse()));

    req.append  ( "GET / HTTP/1.1\r\n" );
    req.append  ( "Connection: Upgrade\r\n" );
    req.append  ( m_host_addr.replace("ws://", "").append("\r\n").prepend("Host: "));
    req.append  ( "Sec-WebSocket-Key: " );
    req.append  ( m_sec_ws_key.append("\r\n") );
    req.append  ( "Sec-WebSocket-Version: 13\r\n" );
    req.append  ( "Upgrade: websocket\r\n" );
    req.append  ( "User-Agent: Qt/5.1.1\r\n\r\n" );

    m_ws_socket->write(req);
}

void OSCQueryClient::onHandshakeResponse()
{
    QTcpSocket* sender = qobject_cast<QTcpSocket*>(QObject::sender());
    QByteArray data;
    QString key;

    while ( sender->bytesAvailable())
        data = sender->readAll();

    // parse accept key
    if ( data.contains("Sec-WebSocket-Accept"))
    {
        // parse the key
        auto spl = data.split('\n');
        for ( const auto& line : spl )
        {
            if ( line.startsWith("Sec-WebSocket-Accept"))
            {
                auto spl2 = line.split(' ');
                key = spl2[1];
                key.replace('\r',"");
            }
        }
    } else return;

    if ( key != m_sec_accept_key )
    {
        qDebug() << "Error: Sec-WebSocket-Accept key is incorrect.";
        return;
    }

    // if response is correct
    // upgrade socket connection to websocket
    QObject::disconnect(m_ws_socket, SIGNAL(readyRead()), this, SLOT(onHandshakeResponse()));
    QObject::connect(m_ws_socket, SIGNAL(readyRead()), this, SLOT(onWebSocketDataReceived()));

    // and send http requests for host info and namespace
    requestHostInfo();
    requestNamespace();
}

void OSCQueryClient::requestHostInfo()
{
    QTcpSocket* tcp = new QTcpSocket(this);
    QByteArray req;
    //req.append();
}

void OSCQueryClient::requestNamespace()
{
    QTcpSocket* tcp = new QTcpSocket(this);

}

void OSCQueryClient::onWebSocketDataReceived()
{
    // coming from the server, it should be unmasked

}

void OSCQueryClient::setHostAddr(QString addr)
{
    qDebug() << "client attempting connection:" << addr;
    m_host_addr = addr;
}

void OSCQueryClient::writeWebSocket(QString address, QVariantList arguments)
{

}


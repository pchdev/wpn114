#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QQmlParserStatus>
#include <QWebSocketServer>
#include <QWebSocket>
#include "device.hpp"

namespace Http
{
enum class Request
{
    NAMESPACE = 0,
    HOST_INFO = 1,
    ATTRIBUTE = 2,
    COMMAND = 3,
    UNKNOWN = 4,
    HANDSHAKE = 5,
};

QPair<Request, QString> parseRequest ( QByteArray data );
QString formatResponse ( QString response );
}

namespace WebSocket
{
enum class Opcodes
{
    CONTINUATION_FRAME = 0x0,
    TEXT_FRAME = 0x1,
    BINARY_FRAME = 0x2,
    CONNECTION_CLOSE = 0x8,
    PING = 0x9,
    PONG = 0xa
};
}
struct HostExtensions
{
    bool access;
    bool value;
    bool range;
    bool description;
    bool tags;
    bool extended_type;
    bool unit;
    bool critical;
    bool clipmode;
    bool listen;
    bool path_changed;
    bool path_removed;
    bool path_added;
    bool path_renamed;

    QJsonObject toJson() const;
};

struct HostSettings
{
    QString name;
    QString osc_transport;
    quint16 osc_port;
    quint16 tcp_port;
    HostExtensions extensions;

    QJsonObject toJson() const;
};

class OSCQueryServer : public OSCQueryDevice, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY ( int tcpPort READ tcpPort WRITE setTcpPort )
    Q_PROPERTY ( int udpPort READ udpPort WRITE setUdpPort )
    Q_PROPERTY ( QString name READ name WRITE setName )

    public:
    OSCQueryServer();

    virtual void writeWebSocket     ( QString addr, QVariantList arguments );
    virtual void componentComplete  ( );
    virtual void classBegin         ( ) {}

    quint16 tcpPort() const { return m_settings.tcp_port; }
    quint16 udpPort() const { return m_settings.osc_port; }
    QString name() const { return m_settings.name; }

    void setTcpPort ( quint16 port ) { m_settings.tcp_port = port; }
    void setUdpPort ( quint16 port ) ;
    void setName    ( QString name ) { m_settings.name = name; }
    void write      ( QTcpSocket* target, QString message );

    protected slots:
    void onNewTcpConnection         ( );
    void onTcpDataReceived          ( );
    void onWebsocketDataReceived    ( QByteArray data );
    void onHandshakeRequest         ( QString key, QTcpSocket* sender);
    void onNamespaceQuery           ( QString , QTcpSocket* sender);
    void onHostInfoQuery            ( QTcpSocket* sender );
    void onQueryCommand             ( QString cmd, QTcpSocket* sender );
    void onAttributeQuery           ( QString attr, QTcpSocket* sender );

    private:    
    QTcpSocket* getWebSocketConnectionFromSender(QTcpSocket* sender);

    HostSettings m_settings;
    QTcpServer* m_tcp_server;
    QVector<QTcpSocket*> m_ws_connections;
};

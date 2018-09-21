#pragma once
#include <QTcpSocket>
#include <QTcpServer>
#include <QString>

enum class Opcodes
{
    CONTINUATION_FRAME = 0x0,
    TEXT_FRAME = 0x1,
    BINARY_FRAME = 0x2,
    CONNECTION_CLOSE = 0x8,
    PING = 0x9,
    PONG = 0xa
};

struct Request
{
    QTcpSocket* con;
    QString req;
};

class WPNWebSocket : public QObject
{
    Q_OBJECT

    public:
    WPNWebSocket( QString host_addr, quint16 port );
    WPNWebSocket( QTcpSocket* con );

    ~WPNWebSocket();

    void connect    ( );
    void connect    ( QString host, quint16 port );
    void disconnect ( );

    void request    ( QString http_req );
    void write      ( QString message );

    QTcpSocket* tcpConnection() { return m_tcp_con; }

    signals:
    void httpMessageReceived    ( QString );
    void textMessageReceived    ( QString );
    void binaryFrameReceived    ( QByteArray );

    void connected              ( );
    void disconnected           ( );

    protected slots:
    void onConnected                  ( );
    void onRawMessageReceived         ( );
    void onHandshakeResponseReceived  ( QString resp );

    protected:
    bool m_mask;
    QString m_device;
    QString m_host_addr;
    quint16 m_host_port;
    QTcpSocket* m_tcp_con;
    QByteArray m_sec_key;
    QByteArray m_accept_key;
    QVector<Request> m_request_queue;
    quint64 m_seed;

    void generateEncryptedSecKey( );
    void decode ( QByteArray data );

    void requestHandshake           ( );
    bool validateHandshake          ( QByteArray data );
};

class WPNWebSocketServer : public QObject
{
    Q_OBJECT

    public:
    WPNWebSocketServer(quint16 port);
    ~WPNWebSocketServer();

    void start  ( );
    void stop   ( );

    void setPort        ( quint16 port ) { m_port = port; }

    signals:
    void httpRequestReceived    ( QTcpSocket*, QString );
    void newConnection          ( WPNWebSocket* );
    void disconnection          ( WPNWebSocket* );

    protected slots:
    void onTcpReadyRead         ( );
    void onNewConnection        ( );
    void onHandshakeRequest     ( QTcpSocket* sender, QByteArray data );

    private:
    void sendHandshakeResponse ( QTcpSocket* target, QString key );

    quint16 m_port;
    QTcpServer* m_tcp_server;
    QVector<WPNWebSocket*> m_connections;
};

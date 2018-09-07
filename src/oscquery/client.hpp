#include "device.hpp"
#include <QTcpSocket>
#include <QQmlParserStatus>

class OSCQueryClient : public OSCQueryDevice, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY  ( QString hostAddr READ hostAddr WRITE setHostAddr NOTIFY hostAddrChanged )
    Q_PROPERTY  ( int port READ port WRITE setPort )

    public:
    OSCQueryClient();

    virtual void componentComplete  ();
    virtual void classBegin         () {}
    virtual void writeWebSocket     ( QString address, QVariantList arguments );

    void requestHostInfo    ( );
    void requestNamespace   ( );

    quint16 port        ( ) const { return m_host_port; }
    QString hostAddr    ( ) const { return m_host_addr; }
    void setPort        ( quint16 port );
    void setHostAddr    ( QString addr );

    signals:
    void hostAddrChanged();

    protected slots:
    void requestHandshake           ( );
    void generateSecKey             ( );
    void onWebSocketDataReceived    ( );
    void onHandshakeResponse        ( );
    void onHostInfoReceived         ( );
    void onNamespaceReceived        ( );

    private:
    quint16 m_host_port;
    QByteArray m_sec_ws_key;
    QByteArray m_sec_accept_key;
    QTcpSocket* m_ws_socket;
    QString m_host_addr;
};

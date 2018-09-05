#include "device.hpp"
#include <QWebSocket>

class OSCQueryClient : public OSCQueryDevice
{
    Q_OBJECT
    Q_PROPERTY  ( QString hostAddr READ hostAddr WRITE setHostAddr NOTIFY hostAddrChanged )

    public:
    OSCQueryClient();
    virtual void sendMessageWS  ( QString address, QVariantList arguments );

    QString hostAddr    ( ) const { return m_host_addr; }
    void setHostAddr    ( QString addr );

    signals:
    void hostAddrChanged();

    protected slots:
    void onWSMessage(QString msg);
    void onBinaryMessage(QByteArray msg);
    void onNewConnection();
    void onDisconnection();
    void onError(QAbstractSocket::SocketError);
    void onStateChanged(QAbstractSocket::SocketState);

    private:
    QString         m_host_addr;
    QWebSocket*     m_ws_hdl;
};

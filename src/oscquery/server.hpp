#pragma once
#include "device.hpp"

#include <QWebSocket>
#include <QWebSocketServer>

struct Client
{
    QWebSocket* ws;
    QString host;
    qint16 osc_port;
};

class OSCQueryServer : public OSCQueryDevice
{
    Q_OBJECT

    Q_PROPERTY  ( int wsPort READ wsPort WRITE setWsPort NOTIFY wsPortChanged )

    public:
    OSCQueryServer();
    virtual void sendMessageWS  ( QString address, QVariantList arguments );

    uint16_t wsPort () const { return m_ws_port; }
    void setWsPort  (uint16_t port);

    signals:
    void wsPortChanged();

    protected slots:
    void onWSMessage(QString msg);
    void onNewConnection();
    void onDisconnection();

    private:
    void exposeHostInfo(QWebSocket* remote);
    void exposeHostTree(QWebSocket* remote);
    uint16_t m_ws_port;
    QWebSocketServer*   m_ws_hdl;
    QVector<QWebSocket*> m_clients;
};

#pragma once

#include "device.hpp"
#include <src/websocket/websocket.hpp>
#include <QTcpSocket>
#include <QQmlParserStatus>

class OSCQueryClient : public OSCQueryDevice, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY  ( QString hostAddr READ hostAddr WRITE setHostAddr NOTIFY hostAddrChanged )
    Q_PROPERTY  ( int port READ port WRITE setPort )

    public:
    OSCQueryClient();
    OSCQueryClient(WPNWebSocket* con);

    virtual void componentComplete  ();
    virtual void classBegin         () {}

    void writeOsc           ( QString method, QVariantList arguments );
    void writeWebSocket     ( QString method, QVariantList arguments );
    void writeWebSocket     ( QString message );
    void writeWebSocket     ( QJsonObject message );

    quint16 port        ( ) const { return m_host_port; }
    QString hostAddr    ( ) const { return m_host_addr; }
    void setPort        ( quint16 port );
    void setHostAddr    ( QString addr );

    signals:
    void connected          ( );
    void disconnected       ( );
    void hostAddrChanged    ( );
    void hostInfoRequest    ( );
    void valueUpdate        ( QString mtd, QVariantList arg );
    void commandRequest     ( QString );

    protected slots:
    void onConnected ( );
    void onTextMessageReceived(QString message);

    private:
    WPNWebSocket* m_ws_con;
    quint16 m_host_port;
    QString m_host_addr;
};

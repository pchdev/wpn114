#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QQmlParserStatus>
#include "client.hpp"
#include "../websocket/websocket.hpp"
#include <qzeroconf.h>

class WPNQueryServer : public WPNDevice, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY ( int tcpPort READ tcpPort WRITE setTcpPort )
    Q_PROPERTY ( int udpPort READ udpPort WRITE setUdpPort )
    Q_PROPERTY ( QString name READ name WRITE setName )

    public:
    WPNQueryServer();

    virtual void componentComplete  ( );
    virtual void classBegin         ( ) {}
    virtual void pushNodeValue      ( WPNNode* node );

    quint16 tcpPort() const { return m_settings.tcp_port; }
    quint16 udpPort() const { return m_settings.osc_port; }
    QString name() const { return m_settings.name; }

    void setTcpPort ( quint16 port );
    void setUdpPort ( quint16 port );
    void setName    ( QString name ) { m_settings.name = name; }

    signals:
    void unknownMethodRequested ( QString method );

    protected slots:
    void onCommand              ( QJsonObject command_obj );
    void onNewConnection        ( WPNWebSocket* client );
    void onHttpRequest          ( QTcpSocket* sender, QString req );
    void onHostInfoRequest      ( QTcpSocket* sender );
    void onNamespaceRequest     ( QTcpSocket* sender, QString method );

    private:        
    OSCHandler* m_osc_hdl;
    HostSettings m_settings;
    WPNWebSocketServer* m_ws_server;
    QVector<WPNQueryClient*> m_clients;
    QZeroConf m_zeroconf;
};

#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QQmlParserStatus>
#include <QWebSocketServer>
#include <QWebSocket>
#include "client.hpp"
#include "../websocket/websocket.hpp"

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
    bool osc_streaming;

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
    virtual void pushNodeValue    ( WPNNode* node );

    quint16 tcpPort() const { return m_settings.tcp_port; }
    quint16 udpPort() const { return m_settings.osc_port; }
    QString name() const { return m_settings.name; }

    void setTcpPort ( quint16 port );
    void setUdpPort ( quint16 port );
    void setName    ( QString name ) { m_settings.name = name; }

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
};

#pragma once

#include "device.hpp"
#include <src/websocket/websocket.hpp>
#include <src/osc/osc.hpp>
#include <QTcpSocket>
#include <QQmlParserStatus>
#include <qzeroconf.h>
#include <QNetworkAccessManager>

class WPNQueryClient : public WPNDevice, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY  ( QString hostAddr READ hostAddr WRITE setHostAddr NOTIFY hostAddrChanged )
    Q_PROPERTY  ( int port READ port WRITE setPort )
    Q_PROPERTY  ( QString zeroConfHost READ zeroConfHost WRITE setZeroConfHost )

    public:
    WPNQueryClient();
    WPNQueryClient(WPNWebSocket* con);

    virtual void componentComplete  ( );
    virtual void classBegin         ( ) {}
    virtual void pushNodeValue    ( WPNNode* node );

    Q_INVOKABLE void requestHttp  ( QString address );
    Q_INVOKABLE void sendMessage  ( QString address, QVariant arguments, bool critical );

    void writeOsc           ( QString method, QVariant arguments );
    void writeWebSocket     ( QString method, QVariant arguments );
    void writeWebSocket     ( QString message );
    void writeWebSocket     ( QJsonObject message );

    QTcpSocket* tcpConnection ();

    quint16 port        ( ) const { return m_host_port; }
    QString hostAddr    ( ) const { return m_host_addr; }
    void setOscPort     ( quint16 port );
    void setPort        ( quint16 port );
    void setHostAddr    ( QString addr );

    QString zeroConfHost ( ) const { return m_zconf_host; }
    void setZeroConfHost ( QString host ) { m_zconf_host = host; }

    signals:
    void treeComplete           ( );
    void hostAddrChanged        ( );
    void connected              ( );
    void disconnected           ( );
    void valueUpdate            ( QJsonObject );
    void command                ( QJsonObject );
    void httpMessageReceived    ( QString message );
    void valueUpdate            ( QString, QVariant );

    protected slots:
    void onHttpReply            ( QNetworkReply* );
    void onZeroConfServiceAdded ( QZeroConfService srv );
    void onHostInfoReceived     ( QJsonObject host_info );
    void onNamespaceReceived    ( QJsonObject nspace );
    void requestStreamStart     ( );

    void onConnected             ( );
    void onBinaryMessageReceived ( QByteArray message );
    void onTextMessageReceived   ( QString message );

    private:
    bool m_direct;
    QNetworkAccessManager* m_http_manager;
    QZeroConf m_zconf;
    QString m_zconf_host;
    OSCHandler* m_osc_hdl;
    WPNWebSocket* m_ws_con;
    quint16 m_host_port;
    QString m_host_addr;
    QString m_host_url;
    HostSettings m_settings;
};

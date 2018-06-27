#ifndef OSCQUERYHANDLER_HPP
#define OSCQUERYHANDLER_HPP

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QQmlParserStatus>
#include "oschandler.h"

class OSCQueryDevice;

class QueryParameter : public QObject, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY  ( QString address READ address WRITE setAddress NOTIFY addressChanged )
    Q_PROPERTY  ( QVariant value READ value WRITE setValue NOTIFY valueChanged )
    Q_PROPERTY  ( QMetaType::Type type READ type WRITE setType )
    Q_PROPERTY  ( bool critical READ critical WRITE setCritical )
    Q_PROPERTY  ( OSCQueryDevice* device READ device WRITE setDevice NOTIFY deviceChanged )

    public:
    QueryParameter();
    ~QueryParameter();

    virtual void componentComplete();
    virtual void classBegin();

    QString address             ( ) const { return m_address; }
    QVariant value              ( ) const { return m_value; }
    bool critical               ( ) const { return m_critical; }
    OSCQueryDevice* device      ( ) const { return m_device; }
    QMetaType::Type type        ( ) const { return m_type; }

    void setAddress   ( QString address );
    void setValue     ( QVariant value );
    void setValueFromNetwork ( QVariant value );
    void setCritical  ( bool critical );
    void setDevice    ( OSCQueryDevice* device );
    void setType      ( QMetaType::Type type );

    signals:
    void addressChanged     ( QString );
    void valueChanged       ( QVariant );
    void valueReceived      ( );
    void deviceChanged      ( OSCQueryDevice* );

    private:
    QString m_address;
    QMetaType::Type m_type;
    QVariant m_value;
    OSCQueryDevice* m_device;
    bool m_critical;

};

class OSCQueryDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY  ( int oscPort READ oscPort WRITE setOscPort NOTIFY oscPortChanged )
    Q_PROPERTY  ( QString deviceName READ deviceName WRITE setDeviceName )

    public:
    virtual void sendMessageWS  ( QString address, QVariantList arguments ) = 0;
    void sendMessageUDP ( QString address, QVariantList arguments );

    void registerParameter      ( QueryParameter* parameter );
    void unRegisterParameter    ( QueryParameter* parameter );

    uint16_t oscPort            ( ) const;
    void setOscPort             ( uint16_t port );
    QString deviceName          ( ) const;
    void setDeviceName          ( QString name );

    signals:
    void clientConnected        ( QString );
    void clientDisconnected     ( QString );
    void messageReceived        ( QString address, QVariantList arguments );
    void oscPortChanged         ( );

    protected:
    QString m_name;
    OSCHandler* m_osc_hdl;
    QVector<QueryParameter*> m_parameters;
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
    uint16_t m_ws_port;
    QWebSocketServer*   m_ws_hdl;
    QVector<QWebSocket*> m_clients;
};

class OSCQueryClient : public QObject
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
    void onNewConnection();
    void onDisconnection();

    private:
    QString         m_host_addr;
    QWebSocket*     m_ws_hdl;
};

#endif // OSCQUERYHANDLER_HPP

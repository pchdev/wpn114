#ifndef OSCHANDLER_H
#define OSCHANDLER_H

#include <QObject>
#include <QUdpSocket>
#include <QQmlParserStatus>

class OSCHandler : public QObject, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY ( int localPort READ localPort WRITE setLocalPort NOTIFY localPortChanged )
    Q_PROPERTY ( int remotePort READ remotePort WRITE setRemotePort NOTIFY remotePortChanged )
    Q_PROPERTY ( QString remoteAddress READ remoteAddress WRITE setRemoteAddress NOTIFY remoteAddressChanged )

    public:
    OSCHandler();

    virtual void componentComplete();
    virtual void classBegin() {}

    uint16_t localPort() const      { return m_local_port; }
    uint16_t remotePort() const     { return m_remote_port; }
    QString remoteAddress() const   { return m_remote_address; }

    void setLocalPort               ( uint16_t port );
    void setRemotePort              ( uint16_t port );
    void setRemoteAddress           ( QString address );

    Q_INVOKABLE void sendMessage    ( QString address, QVariantList arguments );

    protected slots:
    void readPendingDatagrams   ( );
    void readOSCMessage         ( QByteArray message );
    void readOSCBundle          ( QByteArray bundle );

    signals:
    void localPortChanged       ( );
    void remotePortChanged      ( );
    void remoteAddressChanged   ( );
    void messageReceived        ( QString address, QVariantList arguments );

    private:
    uint16_t m_local_port;
    uint16_t m_remote_port;
    QString m_remote_address;
    QUdpSocket* m_udpsocket;

};

#endif // OSCHANDLER_H

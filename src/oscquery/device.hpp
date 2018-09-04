#pragma once
#include "node.hpp"
#include <src/osc/osc.hpp>

class OSCQueryDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY  ( int oscPort READ oscPort WRITE setOscPort NOTIFY oscPortChanged )
    Q_PROPERTY  ( QString deviceName READ deviceName WRITE setDeviceName )

    public:

    OSCQueryDevice      ( );
    ~OSCQueryDevice     ( );

    virtual void sendMessageWS      ( QString address, QVariantList arguments ) = 0;
    void sendMessageUDP             ( QString address, QVariantList arguments );

    void addNode                    ( QueryNode* node );
    void removeNode                 ( QueryNode* node );

    uint16_t oscPort            ( ) const;
    QString deviceName          ( ) const;
    void setOscPort             ( uint16_t port );
    void setDeviceName          ( QString name );

    Q_INVOKABLE QueryNode* getRootNode  ( );
    Q_INVOKABLE QueryNode* getNode      ( QString address );

    Q_INVOKABLE void explore    ( ) const;

    signals:
    void connected              ( QString );
    void disconnected           ( QString );
    void messageReceived        ( QString address, QVariantList arguments );
    void oscPortChanged         ( );

    protected:
    QString             m_name;
    OSCHandler*         m_osc_hdl;
    QueryNode*          m_root_node;
};

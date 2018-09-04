#pragma once
#include "node.hpp"
#include <src/osc/osc.hpp>

class QueryNode;

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

    static void         addNode           ( OSCQueryDevice* dev, QueryNode* node );
    static QueryNode*   findOrCreateNode  ( OSCQueryDevice* dev, QString path );
    static QueryNode*   getNode           ( QString path );

    uint16_t oscPort            ( ) const { return m_osc_hdl->localPort(); }
    QString deviceName          ( ) const { return m_name; }
    QueryNode* rootNode         ( ) { return m_root_node; }

    void setOscPort             ( uint16_t port );
    void setDeviceName          ( QString name );

    Q_INVOKABLE void explore         ( ) const;

    signals:
    void connected              ( QString );
    void disconnected           ( QString );
    void messageReceived        ( QString path, QVariantList arguments );
    void oscPortChanged         ( );

    protected:
    QString             m_name;
    OSCHandler*         m_osc_hdl;
    QueryNode*          m_root_node;
};

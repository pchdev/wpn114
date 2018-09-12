#pragma once

#include "node.hpp"
#include <QObject>

class WPNNode;

class WPNDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY  ( QString deviceName READ deviceName WRITE setDeviceName )

    public:
    WPNDevice  ( );
    ~WPNDevice ( );

    virtual void pushNodeValue(WPNNode* node) = 0;

    static void       addNode           ( WPNDevice* dev, WPNNode* node );
    static WPNNode*   findOrCreateNode  ( WPNDevice* dev, QString path );
    static WPNNode*   getNode           ( QString path );

    QString deviceName          ( ) const { return m_name; }
    WPNNode* rootNode           ( ) { return m_root_node; }
    void setDeviceName          ( QString name );
    Q_INVOKABLE void explore    ( ) const;

    public slots:
    void onValueUpdate  ( QString method, QVariantList arguments );
    void onValueUpdate  ( QJsonObject obj );

    protected:
    QString   m_name;
    WPNNode*  m_root_node;
};

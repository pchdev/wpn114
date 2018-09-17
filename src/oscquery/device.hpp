#pragma once

#include "node.hpp"
#include <QObject>

class WPNNode;

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

class WPNDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY  ( QString deviceName READ deviceName WRITE setDeviceName )
    Q_PROPERTY ( bool singleDevice READ singleDevice WRITE setSingleDevice )

    public:
    WPNDevice  ( );
    ~WPNDevice ( );

    static WPNDevice* instance();
    bool singleDevice() { return m_singleDevice; }
    void setSingleDevice(bool single);

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
    bool m_singleDevice;
    static WPNDevice* m_singleton;

    QString   m_name;
    WPNNode*  m_root_node;
};

#pragma once

#include "node.hpp"
#include "nodetree.hpp"
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
    bool html;
    bool echo;

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

    WPNNode* findOrCreateNode   ( QString path );
    void removeNode             ( QString path );

    QString deviceName          ( ) const { return m_name; }
    WPNNode* rootNode           ( ) { return m_root_node; }
    void setDeviceName          ( QString name );

    Q_INVOKABLE void explore    ( ) const;   
    Q_INVOKABLE QVariant value  ( QString method ) const;
    Q_INVOKABLE WPNNode* get    ( QString path );    

    Q_INVOKABLE WPNNodeTree* nodeTree ( ) const { return m_node_tree; }

    void link(WPNNode* node);

    signals:
    void nodeAdded      ( WPNNode* );
    void nodeRemoved    ( QString );

    public slots:
    void onValueUpdate  ( QString method, QVariant arguments );

    protected:
    bool m_singleDevice;
    static WPNDevice* m_singleton;

    QString       m_name;
    WPNNode*      m_root_node;
    WPNNodeTree*  m_node_tree;
};

#include "device.hpp"
#include <QtDebug>

QJsonObject HostExtensions::toJson() const
{
    QJsonObject ext
    {
        { "OSC_STREAMING", osc_streaming },
        { "ACCESS", access },
        { "VALUE", value },
        { "RANGE", range },
        { "DESCRIPTION", description },
        { "TAGS", tags },
        { "EXTENDED_TYPE", extended_type },
        { "UNIT", unit },
        { "CRITICAL", critical },
        { "CLIPMODE", clipmode },
        { "LISTEN", listen },
        { "PATH_CHANGED", path_changed },
        { "PATH_REMOVED", path_removed },
        { "PATH_ADDED", path_added },
        { "PATH_RENAMED", path_renamed },
        { "HTML", html },
        { "ECHO", echo }
    };

    return ext;
}

QJsonObject HostSettings::toJson() const
{
    QJsonObject obj
    {
        { "NAME", name },
        { "OSC_PORT", osc_port },
        { "OSC_TRANSPORT", osc_transport },
        { "EXTENSIONS", extensions.toJson() }
    };

    return obj;
}

WPNDevice* WPNDevice::m_singleton;
WPNDevice* WPNDevice::instance()
{
    return m_singleton;
}

WPNDevice::WPNDevice()
{
    m_root_node = new WPNNode;
    m_root_node ->setPath   ( "/" );
}

WPNDevice::~WPNDevice() {}

void WPNDevice::setSingleDevice(bool single)
{
    if ( single ) m_singleton = this;
}

void WPNDevice::addNode(WPNDevice* dev, WPNNode *node)
{
    // get node's parent
    QStringList spl = node->path().split('/');
    spl.removeLast();
    QString path = spl.join('/');

    auto parent = findOrCreateNode(dev, path);
    parent->addSubnode(node);

    dev->nodeAdded(node);
}

WPNNode* WPNDevice::findOrCreateNode(WPNDevice* dev, QString path)
{
    QStringList split = path.split('/');
    split.removeFirst(); // first is space

    WPNNode* target = dev->rootNode();

    for ( const auto& node : split )
    {
        for ( const auto& sub : target->subnodes() )
            if ( sub->name() == node )
            {
                target = sub;
                goto next;
            }

        target = target->createSubnode(node);
        next: ;
    }

    return target;
}

WPNNode* WPNDevice::getNode(QString path)
{
    return nullptr;
}

void WPNDevice::onValueUpdate(QJsonObject obj)
{

}

void WPNDevice::onValueUpdate(QString method, QVariant arguments)
{
    auto node = m_root_node->subnode(method);
    if ( node ) node->setValueQuiet(arguments);
    else qDebug() << "[WPNDEVICE] Node" << method << "not found";
}

void WPNDevice::setDeviceName(QString name)
{
    m_name = name;
}

void WPNDevice::explore() const
{
    m_root_node->post();
}

QVariant WPNDevice::value(QString method) const
{
    auto node = WPNDevice::getNode(method);
    if ( node ) return node->value();
    else return QVariant();
}

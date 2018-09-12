#include "device.hpp"


WPNDevice::WPNDevice()
{
    m_root_node = new WPNNode;

    m_root_node ->setPath   ( "/" );
    m_root_node ->setType   ( Type::None );
}

WPNDevice::~WPNDevice() {}

void WPNDevice::addNode(WPNDevice* dev, WPNNode *node)
{
    // get node's parent

    QStringList spl = node->path().split('/');
    spl.removeLast();
    QString path = spl.join('/');

    auto parent = findOrCreateNode(dev, path);
    parent->addSubnode(node);
}

WPNNode* WPNDevice::findOrCreateNode(WPNDevice* dev, QString path)
{
    QStringList split = path.split('/');
    split.removeFirst(); // first is space

    WPNNode* target = dev->rootNode();

    for ( const auto& node : split )
    {
        for ( const auto& sub : target->subnodes() )
        {
            if ( sub->name() == node )
            {
                target = sub;
                goto next;
            }
        }

        target = target->createSubnode(node);
        next:
        ;
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

void WPNDevice::onValueUpdate(QString method, QVariantList arguments)
{
    m_root_node->subnode(method)->setValueQuiet(arguments);
}

void WPNDevice::setDeviceName(QString name)
{
    m_name = name;
}

void WPNDevice::explore() const
{
    m_root_node->post();
}

#include "device.hpp"


OSCQueryDevice::OSCQueryDevice()
{
    m_osc_hdl   = new OSCHandler();
    m_root_node = new QueryNode;
    m_root_node ->setPath("/");

    m_root_node->setType(QueryNode::Type::None);
}

OSCQueryDevice::~OSCQueryDevice()
{

}

void OSCQueryDevice::addNode(OSCQueryDevice* dev, QueryNode *node)
{
    // get node's parent

    QStringList spl = node->path().split('/');
    spl.removeLast();
    QString path = spl.join('/');

    auto parent = findOrCreateNode(dev, path);
    parent->addSubnode(node);
}

QueryNode* OSCQueryDevice::findOrCreateNode(OSCQueryDevice* dev, QString path)
{
    QStringList split = path.split('/');
    split.removeFirst(); // first is space

    QueryNode* target = dev->rootNode();

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

QueryNode* OSCQueryDevice::getNode(QString path)
{
    return nullptr;
}

void OSCQueryDevice::onValueUpdate(QString method, QVariantList arguments)
{
    auto node = m_root_node->subnode(method);
    node->setValue(arguments);
}

void OSCQueryDevice::writeOSC(QString address, QVariantList arguments)
{
    m_osc_hdl->sendMessage(address, arguments);
}

void OSCQueryDevice::setOscPort(uint16_t port)
{
    m_osc_hdl->setLocalPort(port);
}

void OSCQueryDevice::setDeviceName(QString name)
{
    m_name = name;
}

void OSCQueryDevice::explore() const
{
    m_root_node->post();
}

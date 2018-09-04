#include "device.hpp"


OSCQueryDevice::OSCQueryDevice()
{
    m_osc_hdl   = new OSCHandler();
    m_root_node = new QueryNode;
    m_root_node ->setPath("/");

}

OSCQueryDevice::~OSCQueryDevice()
{

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

}

void OSCQueryDevice::sendMessageUDP(QString address, QVariantList arguments)
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

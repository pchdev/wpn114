#include "device.hpp"


OSCQueryDevice::OSCQueryDevice()
{
    m_osc_hdl   = new OSCHandler();
    m_root_node = new QueryNode;
    m_root_node->setAddress("/");

}

OSCQueryDevice::~OSCQueryDevice()
{

}

void OSCQueryDevice::addNode(QueryNode *node)
{
    m_root_node->addChild(node);
}

void OSCQueryDevice::removeNode(QueryNode *node)
{

}

void OSCQueryDevice::sendMessageUDP(QString address, QVariantList arguments)
{
    m_osc_hdl->sendMessage(address, arguments);
}

uint16_t OSCQueryDevice::oscPort() const
{
    return m_osc_hdl->localPort();
}

void OSCQueryDevice::setOscPort(uint16_t port)
{
    m_osc_hdl->setLocalPort(port);
}

QString OSCQueryDevice::deviceName() const
{
    return m_name;
}

void OSCQueryDevice::setDeviceName(QString name)
{
    m_name = name;
}

QueryNode* OSCQueryDevice::getRootNode()
{
    return m_root_node;
}

QueryNode* OSCQueryDevice::getNode(QString address)
{
    return m_root_node->getChild(address);
}

void OSCQueryDevice::explore() const
{
    m_root_node->post();

}

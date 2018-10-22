#include "netexplorer.hpp"

NetExplorer::NetExplorer()
{

}


void NetExplorer::setTarget(QString target)
{
    m_target = target;

    m_zeroconf.startBrowser("_oscjson._tcp");
    QObject::connect( &m_zeroconf, SIGNAL(serviceAdded(QZeroConfService)),
                      this, SLOT(onServiceAdded(QZeroConfService)) );

    QObject::connect( &m_zeroconf, SIGNAL(serviceUpdated(QZeroConfService)),
                      this, SLOT(onServiceUpdated(QZeroConfService)));

    QObject::connect( &m_zeroconf, SIGNAL(serviceRemoved(QZeroConfService)),
                      this, SLOT(onServiceRemoved(QZeroConfService)));

}

void NetExplorer::onServiceAdded(QZeroConfService service)
{
    qDebug() << "[NETEXPLORER] Service added:" << service.name() << service.ip();
    QString host = service.ip().toString()
            .append(":")
            .append(QString::number(service.port()));

    if ( service.name() == m_target &&
         !m_hostlist.contains(host) )
    {
        m_hostlist << host;
        emit newHost(host);
    }
}

void NetExplorer::onServiceUpdated(QZeroConfService service)
{
    qDebug() << "[NETEXPLORER] Service updated:" << service.name() << service.ip();
}

void NetExplorer::onServiceRemoved(QZeroConfService service)
{
    qDebug() << "[NETEXPLORER] Service removed:" << service.name() << service.ip();
    QString host = service.ip().toString()
            .append(":")
            .append(QString::number(service.port()));

    if ( service.name() == m_target &&
         !m_hostlist.contains(host) )
    {
        m_hostlist.removeAll(host);
        emit hostLost(host);
    }
}

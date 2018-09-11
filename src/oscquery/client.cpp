#include "client.hpp"
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QCryptographicHash>
#include <QDataStream>

OSCQueryClient::OSCQueryClient() : OSCQueryDevice()
{
    // direct client
    m_ws_con = new WPNWebSocket("127.0.0.1", 5678);
    QObject::connect(m_ws_con, SIGNAL(connected()), this, SIGNAL(connected()));
    QObject::connect(m_ws_con, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    QObject::connect(m_ws_con, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextMessageReceived(QString)));
}

OSCQueryClient::OSCQueryClient(WPNWebSocket* con)
{
    // indirect client (server image)
    m_ws_con = con;
    QObject::connect(m_ws_con, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextMessageReceived(QString)));
    QObject::connect(m_ws_con, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
}

void OSCQueryClient::componentComplete()
{
    // if direct client: reach host
    m_ws_con->connect();
}

void OSCQueryClient::onTextMessageReceived(QString message)
{
    // - host_info
    // - namespace
    // - value updates (criticals)
    // - ...
    // - signal chains...

}

void OSCQueryClient::writeOsc(QString method, QVariantList arguments)
{

}

void OSCQueryClient::writeWebSocket(QString method, QVariantList arguments)
{

}

void OSCQueryClient::writeWebSocket(QString message)
{
    m_ws_con->write(message);
}

void OSCQueryClient::writeWebSocket(QJsonObject json)
{
    m_ws_con->write(QJsonDocument(json).toJson(QJsonDocument::Compact));
}


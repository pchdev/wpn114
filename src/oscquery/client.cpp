#include "client.hpp"
#include <QJsonDocument>

OSCQueryClient::OSCQueryClient() : m_ws_hdl(new QWebSocket()), OSCQueryDevice()
{
    QObject::connect(m_osc_hdl, SIGNAL(messageReceived(QString,QVariantList)), this, SIGNAL(messageReceived(QString,QVariantList)));
    QObject::connect(m_ws_hdl, SIGNAL(textMessageReceived(QString)), this, SLOT(onWSMessage(QString)));
    QObject::connect(m_ws_hdl, SIGNAL(connected()), this, SLOT(onNewConnection()));
    QObject::connect(m_ws_hdl, SIGNAL(disconnected()), this, SLOT(onDisconnection()));
    QObject::connect(m_ws_hdl, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    QObject::connect(m_ws_hdl, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    QObject::connect(m_ws_hdl, SIGNAL(binaryMessageReceived(QByteArray)), this, SLOT(onBinaryMessage(QByteArray)));
}

void OSCQueryClient::setHostAddr(QString addr)
{
    qDebug() << "client attempting connection:" << addr;
    m_host_addr = addr;
    m_ws_hdl->open(QUrl(addr));
}

void OSCQueryClient::sendMessageWS(QString address, QVariantList arguments)
{
    m_ws_hdl->sendTextMessage(address);
}

void OSCQueryClient::onWSMessage(QString msg)
{
    qDebug() << msg;
    auto jsobj = QJsonDocument::fromJson(msg.toUtf8()).object();
}

void OSCQueryClient::onNewConnection()
{
    emit connected(m_ws_hdl->peerAddress().toString());
    m_ws_hdl->sendTextMessage("HOST_INFO");
}

void OSCQueryClient::onDisconnection()
{
    emit disconnected(m_ws_hdl->peerAddress().toString());
}

void OSCQueryClient::onError(QAbstractSocket::SocketError e)
{
    qDebug() << e;
}

void OSCQueryClient::onStateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << state;
}

void OSCQueryClient::onBinaryMessage(QByteArray msg)
{
    this->onWSMessage(msg);
}

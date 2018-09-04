#include "client.hpp"


OSCQueryClient::OSCQueryClient() : m_ws_hdl(new QWebSocket)
{
    QObject::connect(m_osc_hdl, SIGNAL(messageReceived(QString,QVariantList)), this, SIGNAL(messageReceived(QString,QVariantList)));
    QObject::connect(m_ws_hdl, SIGNAL(textMessageReceived(QString)), this, SLOT(onWSMessage(QString)));
    QObject::connect(m_ws_hdl, SIGNAL(connected()), this, SLOT(onNewConnection()));
    QObject::connect(m_ws_hdl, SIGNAL(disconnected()), this, SLOT(onDisconnection()));
}

void OSCQueryClient::setHostAddr(QString addr)
{
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

    for ( const auto& key : jsobj.keys() )
    {
        if ( key == "OSC_PORT" )
        {
            m_osc_hdl->setRemotePort(jsobj[key].toInt());
            QString reply("/?SET_PORT=");
            reply += QString::number(m_osc_hdl->remotePort());
            reply += "&LOCAL_PORT=";
            reply += QString::number(m_osc_hdl->localPort());

            m_ws_hdl->sendTextMessage("/");
            m_ws_hdl->sendTextMessage(reply);
        }
    }
}

void OSCQueryClient::onNewConnection()
{
    emit connected(m_ws_hdl->peerAddress().toString());
}

void OSCQueryClient::onDisconnection()
{
    emit disconnected(m_ws_hdl->peerAddress().toString());
}

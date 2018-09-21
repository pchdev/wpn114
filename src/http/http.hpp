#pragma once
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QQueue>
#include <QObject>

namespace HTTP
{
struct Reply
{
    QTcpSocket* target;
    QByteArray reply;
};

class ReplyManager : public QObject
{
    Q_OBJECT

    public:

    ReplyManager();
    ~ReplyManager();
    void enqueue(Reply rep);
    static QString formatJsonResponse( QString response );
    static QString formatJsonResponse( QJsonObject obj );
    static QByteArray formatFileResponse( QByteArray file);

    protected slots:
    void onBytesWritten(qint64);
    void next();

    private:
    bool m_free;
    QQueue<Reply> m_queue;
};

}



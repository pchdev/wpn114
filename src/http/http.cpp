#include "http.hpp"
#include <QDateTime>
#include <QtDebug>

QString HTTP::formatJsonResponse( QString response )
{
    QString resp    ( "HTTP/1.1 200 OK\r\n" );
    resp.append     ( QDateTime::currentDateTime().toString("ddd, dd MMMM yyyy hh:mm:ss t" ));
    resp.append     ( "\r\n" );
    resp.append     ( "Server: Qt/5.1.1\r\n" );
    resp.append     ( "Content-Type: application/json\r\n" );
    resp.append     ( "Content-Length: " );
    resp.append     ( QString::number(response.size()+ANDROID_JSON) );
    resp.append     ( "\r\n" );
    resp.append     ( "\r\n" );
    resp.append     ( response );

    return resp;
}

QString HTTP::formatJsonResponse(QJsonObject obj)
{
    return HTTP::formatJsonResponse(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

QString HTTP::formatRequest(QString address, QString attr, QString host)
{
    QString req = "GET ";

    req.append  ( address );

    if ( !attr.isEmpty() ) req.append("?").append(attr);

    req.append(" HTTP/1.1\r\n");

    req.append  ( host.prepend("Host: ")).append("\r\n" );
    req.append  ( "Accept: */*\r\n" );
    req.append  ( "Connection: close\r\n\r\n" );

    return req;
}

QByteArray HTTP::formatFileResponse(QByteArray file)
{
    QByteArray resp ( "HTTP/1.1 200 OK\r\n" );
    resp.append     ( QDateTime::currentDateTime().toString("ddd, dd MMMM yyyy hh:mm:ss t" ));
    resp.append     ( "\r\n" );
    resp.append     ( "Server: Qt/5.1.1\r\n" );
    resp.append     ( "Content-Type: text/plain\r\n" );
    resp.append     ( "Content-Length: " );
    resp.append     ( QString::number(file.size()) );
    resp.append     ( "\r\n" );
    resp.append     ( "\r\n" );
    resp.append     ( file );

    return resp;
}

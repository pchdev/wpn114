#include "http.hpp"
#include <QString>
#include <QDateTime>

QString formatResponse( QString response )
{
    QString resp    ( "HTTP/1.1 200 OK\r\n" );
    resp.append     ( QDateTime::currentDateTime().toString("ddd, dd MMMM yyyy hh:mm:ss t" ));
    resp.append     ( "\r\n" );
    resp.append     ( "Server: Qt/5.1.1\r\n" );
    resp.append     ( "Content-Type: application/json\r\n" );
    resp.append     ( "Content-Length: " );
    resp.append     ( QString::number(response.size()) );
    resp.append     ( "\r\n" );
    resp.append     ( "\r\n" );
    resp.append     ( response );

    return resp;
}

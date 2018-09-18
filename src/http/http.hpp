#pragma once
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

namespace HTTP
{
QString formatJsonResponse( QString response );
QString formatJsonResponse( QJsonObject obj );

QByteArray formatFileResponse (QByteArray file);
QString formatRequest(QString address, QString attribute , QString host);
}



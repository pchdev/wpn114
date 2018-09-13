#pragma once
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

namespace HTTP
{
QString formatJsonResponse( QString response );
QString formatJsonResponse( QJsonObject obj );

QString formatRequest(QString address, QString attribute , QString host);
}

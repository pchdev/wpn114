#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "audiobackend.h"
#include "audiomaster.h"
#include "qsine.h"
#include "audioplugin.h"
#include "rooms.h"

#include <QApplication>
#include <QQuickWidget>

int main(int argc, char *argv[])
{

#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    qmlRegisterType<AudioObject>        ( );
    qmlRegisterType<AudioBackend, 1>    ( "WPN114", 1, 0, "AudioOut" );
    qmlRegisterType<AudioMaster, 1>     ( "WPN114", 1, 0, "Master" );
    qmlRegisterType<SinOsc, 1>          ( "WPN114", 1, 0, "SinOsc" );
    qmlRegisterType<AudioPlugin, 1>     ( "WPN114", 1, 0, "AudioPlugin" );
    qmlRegisterType<RoomsSetup, 1>      ( "WPN114", 1, 0, "RoomsSetup" );
    qmlRegisterType<Rooms, 1>           ( "WPN114", 1, 0, "Rooms" );

    QApplication app(argc, argv);
    //QGuiApplication app(argc, argv);


    QQuickWidget* view = new QQuickWidget;
    view->setSource(QUrl::fromLocalFile("/Users/pchd/Repositories/wpn114-qt/main.qml"));
    view->show();

    /*QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;*/



    return app.exec();
}

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "audiobackend.h"
#include "audiomaster.h"
#include "qsine.h"
#include "audioplugin.h"
#include "rooms.h"
#include "fields.h"
#include "oneshots.h"

#include <QApplication>
#include <QQuickWidget>

int main(int argc, char *argv[])
{

#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    qmlRegisterType<AudioObject>        ( );
    qmlRegisterType<AudioEffectObject>  ( );
    qmlRegisterType<AudioSend>          ( "WPN114", 1, 0, "Send" );
    qmlRegisterType<AudioBackend, 1>    ( "WPN114", 1, 0, "AudioOut" );
    qmlRegisterType<AudioMaster, 1>     ( "WPN114", 1, 0, "Master" );
    qmlRegisterType<SinOsc, 1>          ( "WPN114", 1, 0, "SinOsc" );
    qmlRegisterType<AudioPlugin, 1>     ( "WPN114", 1, 0, "AudioPlugin" );
    qmlRegisterType<RoomsSetup, 1>      ( "WPN114", 1, 0, "RoomsSetup" );
    qmlRegisterType<Rooms, 1>           ( "WPN114", 1, 0, "Rooms" );
    qmlRegisterType<Fields, 1>          ( "WPN114", 1, 0, "Fields" );
    qmlRegisterType<Oneshots, 1>        ( "WPN114", 1, 0, "Oneshots" );

    QApplication app(argc, argv);

    QQuickWidget* view = new QQuickWidget;
    view->setSource(QUrl::fromLocalFile("/Users/pchd/Repositories/wpn114/main.qml"));
    view->show();

    return app.exec();
}

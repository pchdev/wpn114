#include "qml_plugin.hpp"
#include <audio_objects/audioplugin/audioplugin.hpp>
#include <audio_objects/fields/fields.hpp>
#include <audio_objects/oneshots/oneshots.hpp>
#include <audio_objects/rooms/rooms.hpp>
#include <audio_objects/sine/sine.hpp>

#include <QQmlEngine>
#include <qqml.h>

void qml_plugin::registerTypes(const char *uri)
{
    qmlRegisterType<AudioObject>        ( );
    qmlRegisterType<AudioEffectObject>  ( );
    qmlRegisterType<AudioSend>          ( "WPN114", 1, 0, "Send" );
    qmlRegisterType<AudioBackend, 1>    ( "WPN114", 1, 0, "AudioOut" );
    qmlRegisterType<AudioMaster, 1>     ( "WPN114", 1, 0, "Master" );
    qmlRegisterType<SinOsc, 1>          ( "WPN114", 1, 0, "SinOsc" );
    qmlRegisterType<AudioPlugin, 1>     ( "WPN114", 1, 0, "AudioPlugin" );
    qmlRegisterType<RoomsSetup, 1>      ( "WPN114", 1, 0, "RoomsSetup" );
    qmlRegisterType<Source, 1>          ( "WPN114", 1, 0, "Source2D" );
    qmlRegisterType<Speaker, 1>         ( "WPN114", 1, 0, "Speaker2D" );
    qmlRegisterType<Rooms, 1>           ( "WPN114", 1, 0, "Rooms2D" );
    qmlRegisterType<Fields, 1>          ( "WPN114", 1, 0, "Fields" );
    qmlRegisterType<Oneshots, 1>        ( "WPN114", 1, 0, "Oneshots" );

}

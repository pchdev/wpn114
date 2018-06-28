#include "qml_plugin.hpp"
//#include <audio_objects/audioplugin/audioplugin.hpp>
//#include <audio_objects/fields/fields.hpp>
//#include <audio_objects/oneshots/oneshots.hpp>
//#include <audio_objects/rooms/rooms.hpp>
//#include <audio_objects/sine/sine.hpp>
//#include <audio_objects/ashes/ashes.hpp>
//#include <audio_objects/sharpen/sharpen.hpp>

#include <src/midi-backend.hpp>
#include <src/oschandler.h>
#include <src/oscqueryhandler.hpp>

#include <QQmlEngine>
#include <qqml.h>

void qml_plugin::registerTypes(const char *uri)
{
//    qmlRegisterType<AudioObject>        ( );
//    qmlRegisterType<AudioEffectObject>  ( );
//    qmlRegisterType<RoomsObject>        ( );
//    qmlRegisterType<SpeakerObject>      ( );

//    qmlRegisterType<AudioSend>          ( "WPN114", 1, 0, "Send" );
//    qmlRegisterType<AudioBackend, 1>    ( "WPN114", 1, 0, "AudioOut" );
//    qmlRegisterType<AudioMaster, 1>     ( "WPN114", 1, 0, "Master" );
//    qmlRegisterType<SinOsc, 1>          ( "WPN114", 1, 0, "SinOsc" );
//    qmlRegisterType<AudioPlugin, 1>     ( "WPN114", 1, 0, "AudioPlugin" );
//    qmlRegisterType<RoomsSetup, 1>      ( "WPN114", 1, 0, "RoomsSetup" );
//    qmlRegisterType<Source, 1>          ( "WPN114", 1, 0, "Source2D" );
//    qmlRegisterType<Speaker, 1>         ( "WPN114", 1, 0, "Speaker2D" );
//    qmlRegisterType<SpeakerPair, 1>     ( "WPN114", 1, 0, "SpeakerPair2D" );
//    qmlRegisterType<SpeakerRing, 1>     ( "WPN114", 1, 0, "SpeakerRing2D" );
//    qmlRegisterType<Rooms, 1>           ( "WPN114", 1, 0, "Rooms2D" );
//    qmlRegisterType<Fields, 1>          ( "WPN114", 1, 0, "Fields" );
//    qmlRegisterType<Oneshots, 1>        ( "WPN114", 1, 0, "Oneshots" );
//    qmlRegisterType<Sharpen, 1>         ( "WPN114", 1, 0, "Sharpen" );
//    qmlRegisterType<Ashes, 1>           ( "WPN114", 1, 0, "Ashes" );

    qmlRegisterType<MIDIHandler, 1>       ( "WPN114", 1, 0, "MIDIHandler" );
    qmlRegisterType<OSCHandler, 1>        ( "WPN114", 1, 0, "OSCHandler" );
    qmlRegisterType<QueryParameter, 1>    ( "WPN114", 1, 0, "QueryParameter" );
    qmlRegisterType<OSCQueryServer, 1>    ( "WPN114", 1, 0, "OSCQueryServer" );
    qmlRegisterType<OSCQueryClient, 1>    ( "WPN114", 1, 0, "OSCQueryClient" );

}

#include "qml_plugin.hpp"
#include <src/midi/midi.hpp>
#include <src/oscquery/client.hpp>
#include <src/oscquery/query-server.hpp>
#include <src/oscquery/node.hpp>
#include <src/oscquery/file.hpp>
#include <src/oscquery/folder.hpp>
#include <src/audio/audio.hpp>
#include <audio_objects/sine/sine.hpp>
#include <audio_objects/stpanner/stereopanner.hpp>
#include <audio_objects/audioplugin/audioplugin.hpp>
#include <QQmlEngine>
#include <qqml.h>

void qml_plugin::registerTypes(const char *uri)
{
    Q_UNUSED    ( uri );

    qmlRegisterUncreatableType<Type, 1>          ( "WPN114", 1, 0, "Type", "Coucou" );
    qmlRegisterUncreatableType<Access, 1>        ( "WPN114", 1, 0, "Access", "Coucou" );
    qmlRegisterUncreatableType<Clipmode, 1>      ( "WPN114", 1, 0, "Clipmode", "Coucou" );
    qmlRegisterUncreatableType<StreamNode, 1>    ( "WPN114", 1, 0, "OutStreamNode","Coucou");

    qmlRegisterType<MIDIHandler, 1>       ( "WPN114", 1, 0, "MIDIHandler" );
    qmlRegisterType<OSCHandler, 1>        ( "WPN114", 1, 0, "OSCHandler" );
    qmlRegisterType<WPNNode, 1>           ( "WPN114", 1, 0, "Node" );
    qmlRegisterType<WPNFileNode, 1>       ( "WPN114", 1, 0, "FileNode" );
    qmlRegisterType<WPNFolderNode, 1>     ( "WPN114", 1, 0, "FolderNode" );
    qmlRegisterType<WPNFolderMirror, 1>   ( "WPN114", 1, 0, "FolderMirror" );
    qmlRegisterType<WPNQueryServer, 1>    ( "WPN114", 1, 0, "OSCQueryServer" );
    qmlRegisterType<WPNQueryClient, 1>    ( "WPN114", 1, 0, "OSCQueryClient" );

    qmlRegisterType<WorldStream, 1>       ( "WPN114", 1, 0, "AudioStream" );
    qmlRegisterType<SinOsc, 1>            ( "WPN114", 1, 0, "SinOsc" );
    qmlRegisterType<StereoPanner, 1>      ( "WPN114", 1, 0, "StereoPanner" );
    qmlRegisterType<AudioPlugin, 1>       ( "WPN114", 1, 0, "AudioPlugin" );
}

#include "qml_plugin.hpp"

#ifdef WPN114_NETWORK
#include <src/oscquery/client.hpp>
#include <src/oscquery/query-server.hpp>
#include <src/oscquery/node.hpp>
#include <src/oscquery/file.hpp>
#include <src/oscquery/folder.hpp>
#endif

#ifdef WPN114_AUDIO
#include <src/audio/audio.hpp>
#include <audio_objects/sine/sine.hpp>
#include <audio_objects/stpanner/stereopanner.hpp>
#include <audio_objects/sampler/sampler.hpp>
#include <audio_objects/rooms/rooms.hpp>
#include <audio_objects/mangler/mangler.hpp>
#include <audio_objects/sharpen/sharpen.hpp>
#include <audio_objects/multisampler/multisampler.hpp>
#include <audio_objects/fork/fork.hpp>
#include <audio_objects/peakrms/peakrms.hpp>
#endif

#ifdef WPN114_MIDI
#include <src/midi/midi.hpp>
#endif

#ifdef WPN114_VST
#include <audio_objects/audioplugin/audioplugin.hpp>
#endif

#include <QQmlEngine>
#include <qqml.h>

void qml_plugin::registerTypes(const char *uri)
{
    Q_UNUSED    ( uri );

#ifdef WPN114_AUDIO
    qmlRegisterUncreatableType<StreamNode, 1>   ( "WPN114", 1, 0, "StreamNode","Coucou");
    qmlRegisterUncreatableType<RoomNode, 1>     ( "WPN114", 1, 0, "RoomNode", "Coucou" );
    qmlRegisterUncreatableType<RoomSource, 1>   ( "WPN114", 1, 0, "RoomSource", "Coucou");
    qmlRegisterType<WorldStream, 1>             ( "WPN114", 1, 0, "AudioStream" );
    qmlRegisterType<SinOsc, 1>                  ( "WPN114", 1, 0, "SinOsc" );
    qmlRegisterType<StereoPanner, 1>            ( "WPN114", 1, 0, "StereoPanner" );
    qmlRegisterType<Sampler, 1>                 ( "WPN114", 1, 0, "Sampler" );
    qmlRegisterType<StreamSampler, 1>           ( "WPN114", 1, 0, "StreamSampler" );
    qmlRegisterType<MultiSampler, 1>            ( "WPN114", 1, 0, "MultiSampler" );
    qmlRegisterType<RoomSetup, 1>               ( "WPN114", 1, 0, "RoomSetup" );
    qmlRegisterType<MonoSource, 1>              ( "WPN114", 1, 0, "MonoSource" );
    qmlRegisterType<StereoSource, 1>            ( "WPN114", 1, 0, "StereoSource" );
    qmlRegisterType<Rooms, 1>                   ( "WPN114", 1, 0, "Rooms" );
    qmlRegisterType<SpeakerPair, 1>             ( "WPN114", 1, 0, "SpeakerPair" );
    qmlRegisterType<SpeakerRing, 1>             ( "WPN114", 1, 0, "SpeakerRing" );
    qmlRegisterType<Sharpen, 1>                 ( "WPN114", 1, 0, "Sharpen" );
    qmlRegisterType<Mangler, 1>                 ( "WPN114", 1, 0, "Mangler" );
    qmlRegisterType<Fork, 1>                    ( "WPN114", 1, 0, "Fork" );
    qmlRegisterType<PeakRMS, 1>                 ( "WPN114", 1, 0, "PeakRMS" );
#endif

#ifdef WPN114_MIDI
    qmlRegisterType<MIDIHandler, 1>             ( "WPN114", 1, 0, "MIDIHandler" );
#endif

#ifdef WPN114_VST
    qmlRegisterType<AudioPlugin, 1>             ( "WPN114", 1, 0, "AudioPlugin" );
#endif

#ifdef WPN114_NETWORK
    qmlRegisterUncreatableType<Type, 1>         ( "WPN114", 1, 0, "Type", "Coucou" );
    qmlRegisterUncreatableType<Access, 1>       ( "WPN114", 1, 0, "Access", "Coucou" );
    qmlRegisterUncreatableType<Clipmode, 1>     ( "WPN114", 1, 0, "Clipmode", "Coucou" );
    qmlRegisterType<OSCHandler, 1>              ( "WPN114", 1, 0, "OSCHandler" );
    qmlRegisterType<WPNNode, 1>                 ( "WPN114", 1, 0, "Node" );
    qmlRegisterType<WPNFileNode, 1>             ( "WPN114", 1, 0, "FileNode" );
    qmlRegisterType<WPNFolderNode, 1>           ( "WPN114", 1, 0, "FolderNode" );
    qmlRegisterType<WPNFolderMirror, 1>         ( "WPN114", 1, 0, "FolderMirror" );
    qmlRegisterType<WPNQueryServer, 1>          ( "WPN114", 1, 0, "OSCQueryServer" );
    qmlRegisterType<WPNQueryClient, 1>          ( "WPN114", 1, 0, "OSCQueryClient" );
#endif
}

#include "qml_plugin.hpp"
#include <src/midi/midi.hpp>
#include <src/oscquery/client.hpp>
#include <src/oscquery/query-server.hpp>
#include <src/oscquery/node.hpp>

#include <QQmlEngine>
#include <qqml.h>

void qml_plugin::registerTypes(const char *uri)
{
    Q_UNUSED    ( uri );

    qmlRegisterType<MIDIHandler, 1>       ( "WPN114", 1, 0, "MIDIHandler" );
    qmlRegisterType<OSCHandler, 1>        ( "WPN114", 1, 0, "OSCHandler" );
    qmlRegisterType<WPNNode, 1>           ( "WPN114", 1, 0, "Node" );
    qmlRegisterType<WPNQueryServer, 1>    ( "WPN114", 1, 0, "OSCQueryServer" );
    qmlRegisterType<WPNQueryClient, 1>    ( "WPN114", 1, 0, "OSCQueryClient" );
}

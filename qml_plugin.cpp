#include "qml_plugin.hpp"
#include <src/midi/midi.hpp>
#include <src/oscquery/client.hpp>
#include <src/oscquery/server.hpp>
#include <src/oscquery/node.hpp>

#include <QQmlEngine>
#include <qqml.h>

void qml_plugin::registerTypes(const char *uri)
{
    qmlRegisterType<MIDIHandler, 1>       ( "WPN114", 1, 0, "MIDIHandler" );
    qmlRegisterType<OSCHandler, 1>        ( "WPN114", 1, 0, "OSCHandler" );
    qmlRegisterType<QueryNode, 1>         ( "WPN114", 1, 0, "QueryNode" );
    qmlRegisterType<OSCQueryServer, 1>    ( "WPN114", 1, 0, "OSCQueryServer" );
    qmlRegisterType<OSCQueryClient, 1>    ( "WPN114", 1, 0, "OSCQueryClient" );
}

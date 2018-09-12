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

    qmlRegisterUncreatableType<Type, 1>         ( "WPN114", 1, 0, "Type", "Coucou" );
    qmlRegisterUncreatableType<Access, 1>       ( "WPN114", 1, 0, "Access", "Coucou" );
    qmlRegisterUncreatableType<Clipmode, 1>     ( "WPN114", 1, 0, "Clipmode", "Coucou" );

    qmlRegisterType<MIDIHandler, 1>       ( "WPN114", 1, 0, "MIDIHandler" );
    qmlRegisterType<OSCHandler, 1>        ( "WPN114", 1, 0, "OSCHandler" );
    qmlRegisterType<WPNNode, 1>           ( "WPN114", 1, 0, "Node" );
    qmlRegisterType<WPNQueryServer, 1>    ( "WPN114", 1, 0, "OSCQueryServer" );
    qmlRegisterType<WPNQueryClient, 1>    ( "WPN114", 1, 0, "OSCQueryClient" );
}

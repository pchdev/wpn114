cmake_minimum_required(VERSION 2.8)

project ( WPN114 )

set ( CMAKE_INCLUDE_CURRENT_DIR ON )
set ( CMAKE_AUTOMOC ON )

set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake_modules )

set( PROJECT_HDRS
    qml_plugin.hpp
    src/midi/midi.hpp
    src/osc/osc.hpp
    src/midi/RtMidi.h
    src/oscquery/device.hpp
    src/oscquery/client.hpp
    src/oscquery/node.hpp
    src/oscquery/query-server.hpp
    src/http/http.hpp
    src/websocket/websocket.hpp
    src/oscquery/file.hpp
    src/oscquery/folder.hpp
 )

set ( PROJECT_SRCS
    qml_plugin.cpp
    src/midi/midi.cpp
    src/osc/osc.cpp
    src/midi/RtMidi.cpp
    src/oscquery/device.cpp
    src/oscquery/client.cpp
    src/oscquery/node.cpp
    src/oscquery/query-server.cpp
    src/http/http.cpp
    src/websocket/websocket.cpp
    src/oscquery/file.cpp
    src/oscquery/folder.cpp
)

if ( APPLE )
    add_definitions(-D__MACOSX_CORE__)
endif()

find_package( Sndfile REQUIRED )
find_package( Qt5QuickWidgets REQUIRED )
find_package( Qt5Multimedia REQUIRED)
find_package( Qt5Widgets REQUIRED )
find_package( Qt5WebSockets REQUIRED )

add_library(${PROJECT_NAME} SHARED ${PROJECT_HDRS} ${PROJECT_SRCS})

target_link_libraries( ${PROJECT_NAME} Qt5::QuickWidgets Qt5::Multimedia Qt5::Widgets Qt5::WebSockets )
target_link_libraries( ${PROJECT_NAME} Sndfile )

if ( ZEROCONF )
    target_include_directories( ${PROJECT_NAME} PUBLIC ${ZEROCONF_INCLUDE_DIR} )
    target_link_libraries( ${PROJECT_NAME} ${ZEROCONF_LIB})
endif()

if ( APPLE )
    target_link_libraries( ${PROJECT_NAME} "-framework CoreFoundation" )
    target_link_libraries( ${PROJECT_NAME} "-framework CoreAudio" )
    target_link_libraries( ${PROJECT_NAME} "-framework CoreMIDI" )
endif( )

cmake_minimum_required(VERSION 2.8)

project ( WPN114 )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)


set( PROJECT_HDRS
    qml_plugin.hpp
    audio_objects/audioplugin/audioplugin.hpp
    audio_objects/audioplugin/aeffect.h
    audio_objects/audioplugin/aeffectx.h
    audio_objects/fields/fields.hpp
    audio_objects/oneshots/oneshots.hpp
    audio_objects/rooms/rooms.hpp
    audio_objectS/sine/sine.hpp
    src/audiobackend.hpp
    src/audiomaster.hpp
    src/wpnsndfile.hpp
 )

set ( PROJECT_SRCS
    qml_plugin.cpp
    audio_objects/audioplugin/audioplugin.mm
    audio_objects/fields/fields.cpp
    audio_objects/oneshots/oneshots.cpp
    audio_objects/rooms/rooms.cpp
    audio_objects/sine/sine.cpp
    src/audiobackend.cpp
    src/audiomaster.cpp
    src/wpnsndfile.cpp
)

find_package( Qt5QuickWidgets )
find_package( Qt5Multimedia )
find_package( Qt5Widgets )

add_library(${PROJECT_NAME} SHARED ${PROJECT_HDRS} ${PROJECT_SRCS})

target_link_libraries( ${PROJECT_NAME} Qt5::QuickWidgets Qt5::Multimedia Qt5::Widgets )

if(APPLE)
    target_link_libraries( ${PROJECT_NAME} "-framework CoreFoundation" )
endif()

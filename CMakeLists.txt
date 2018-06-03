cmake_minimum_required(VERSION 2.8)

project ( WPN114 )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake_modules )


set( PROJECT_HDRS
    qml_plugin.hpp
    #audio_objects/ashes/ashes.hpp
    #audio_objects/audioplugin/audioplugin.hpp
    #audio_objects/audioplugin/aeffect.h
    #audio_objects/audioplugin/aeffectx.h
    #audio_objects/fields/fields.hpp
    #audio_objects/oneshots/oneshots.hpp
    #audio_objects/rooms/rooms.hpp
    #audio_objects/sine/sine.hpp
    #audio_objects/sharpen/sharpen.hpp
    #src/audiobackend.hpp
    #src/audiomaster.hpp
    #src/wpnsndfile.hpp
    src/midi-backend.hpp
    src/RtMidi.h
 )

set ( PROJECT_SRCS
    qml_plugin.cpp
    #audio_objects/ashes/ashes.cpp
    #audio_objects/audioplugin/audioplugin.mm
    #audio_objects/fields/fields.cpp
    #audio_objects/oneshots/oneshots.cpp
    #audio_objects/rooms/rooms.cpp
    #audio_objects/sine/sine.cpp
    #audio_objects/sharpen/sharpen.cpp
    #src/audiobackend.cpp
    #src/audiomaster.cpp
    #src/wpnsndfile.cpp
    src/midi-backend.cpp
    src/RtMidi.cpp
)

if ( APPLE )
    add_definitions(-D__MACOSX_CORE__)
endif()

find_package( Sndfile REQUIRED )
find_package( Qt5QuickWidgets REQUIRED )
find_package( Qt5Multimedia REQUIRED)
find_package( Qt5Widgets REQUIRED )

add_library(${PROJECT_NAME} SHARED ${PROJECT_HDRS} ${PROJECT_SRCS})

target_link_libraries( ${PROJECT_NAME} Qt5::QuickWidgets Qt5::Multimedia Qt5::Widgets )
target_link_libraries( ${PROJECT_NAME} Sndfile )

if(APPLE)
    target_link_libraries( ${PROJECT_NAME} "-framework CoreFoundation" )
    target_link_libraries( ${PROJECT_NAME} "-framework CoreAudio" )
    target_link_libraries( ${PROJECT_NAME} "-framework CoreMIDI" )
endif()

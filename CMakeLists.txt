project(quarre-core C CXX)

cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(WPN_AUDIO "Enables audio functionnalities" ON)
option(WPN_AUDIO_TRACKS "Enables audio tracks support" ON)
option(WPN_AUDIO_AUX "Enables audio aux/sends support" ON)
option(WPN_AUDIO_SNDFILE "Enables soundfile support" ON)
option(WPN_AUDIO_PLUGINS_AMBISONICS "Enables ambisonics support" OFF)
option(WPN_AUDIO_PLUGINS_ASH "Enables pink/white noise support" OFF)
option(WPN_AUDIO_PLUGINS_BURSTS "Enables pink noise audio tests bursts" OFF)
option(WPN_AUDIO_PLUGINS_CIRCLES "Enables simple circular spatialization support" OFF)
option(WPN_AUDIO_PLUGINS_FIELDS "Enables fields unit" ON)
option(WPN_AUDIO_PLUGINS_GRANARY "Enables granular synthesis support" OFF)
option(WPN_AUDIO_PLUGINS_ONESHOTS "Enables one-shot soundfile player support" ON)
option(WPN_AUDIO_PLUGINS_POLY_ONESHOTS "Enables poly one-shots sfplayer support" OFF)
option(WPN_AUDIO_PLUGINS_ROOMS "Enables rooms spatialization system" ON)
option(WPN_AUDIO_PLUGINS_SOUNDTRACKS "Enables soundtracks plugin" ON)
option(WPN_AUDIO_PLUGINS_VBAP "Enables vbap support" OFF)
option(WPN_AUDIO_PLUGINS_VST "Enables support for VST plugins" ON)

if( WPN_AUDIO_PLUGINS_FIELDS OR
    WPN_AUDIO_PLUGINS_GRANARY OR
    WPN_AUDIO_PLUGINS_ONESHOTS OR
    WPN_AUDIO_PLUGINS_POLY_ONESHOTS OR
    WPN_AUDIO_PLUGINS_SOUNDTRACKS
    )
    set(WPN_AUDIO_SNDFILE ON)
endif()

option(WPN_CONTROL "Enables control functionnalities" ON)
option(WPN_CONTROL_MIDI "Enables midi support with RtMidi" OFF)
option(WPN_CONTROL_OSC "Enables osc support with oscpack (tbi)" OFF)
option(WPN_CONTROL_SERIAL "Enables serial support (tbi)" OFF)
option(WPN_CONTROL_WS "Enables websocket support (tbi)" OFF)
option(WPN_CONTROL_HTTP "Enables http support (tbi)" OFF)
option(WPN_CONTROL_COMMAND_LINE "Enables command-line functionnalities" OFF)
option(WPN_CONTROL_OSSIA "Enables libossia network functionnalities" ON)

option(WPN_CONTROL_MIDI_PLUGINS_PUSH "Enables Ableton Push functionnalities" ON)

add_subdirectory(wpn114)

set(WPN114_INCLUDES ${CMAKE_SOURCE_DIR})

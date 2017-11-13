project(quarre-core C CXX)

cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(WPN_AUDIO "Enables audio functionnalities" ON)
option(WPN_AUDIO_PLUGINS_AMBISONICS "Enables ambisonics support" OFF)
option(WPN_AUDIO_PLUGINS_ASH "Enables pink/white noise support" OFF)
option(WPN_AUDIO_PLUGINS_BURSTS "Enables pink noise audio tests bursts" OFF)
option(WPN_AUDIO_PLUGINS_CIRCLES "Enables simple circular spatialization support" OFF)
option(WPN_AUDIO_PLUGINS_FIELDS "Enables fields unit" OFF)
option(WPN_AUDIO_PLUGINS_GRANARY "Enables granular synthesis support" OFF)
option(WPN_AUDIO_PLUGINS_ONESHOTS "Enables one-shot soundfile player support" ON)
option(WPN_AUDIO_PLUGINS_POLY_ONESHOTS "Enables poly one-shots sfplayer support" OFF)
option(WPN_AUDIO_PLUGINS_VBAP "Enables vbap support" OFF)
option(WPN_AUDIO_PLUGINS_VST "Enables support for VST plugins" ON)

option(WPN_CONTROL "Enables controller functionnalities" OFF)
option(WPN_CONTROL_PUSH "Enables Ableton Push functionnalities" OFF)
option(WPN_COMMAND_LINE "Enables command-line functionnalities" OFF)
option(WPN_OSSIA "Enables libossia network functionnalities" ON)

add_subdirectory(wpn114)

set(WPN114_INCLUDES ${CMAKE_SOURCE_DIR})

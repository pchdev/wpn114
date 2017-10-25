project(quarre-core C CXX)

cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(WPN_AUDIO "Enables audio functionnalities" ON)
option(WPN_AUDIO_PLUGINS_AMBISONICS "Enables ambisonics support" ON)
option(WPN_AUDIO_PLUGINS_ASH "Enables pink/white noise suppor" ON)
option(WPN_AUDIO_PLUGINS_BURSTS "Enables pink noise audio tests bursts" ON)
option(WPN_AUDIO_PLUGINS_FIELDS "Enables fields unit" ON)
option(WPN_AUDIO_PLUGINS_GRANARY "Enables granular synthesis support" ON)
option(WPN_AUDIO_PLUGINS_VBAP "Enables vbap support" ON)
option(WPN_AUDIO_PLUGINS_VST "Enables support for VST plugins" ON)

option(WPN_CONTROL "Enables controller functionnalities" OFF)
option(WPN_CONTROL_PUSH "Enables Ableton Push functionnalities" OFF)
option(WPN_COMMAND_LINE "Enables command-line functionnalities" OFF)
option(WPN_OSSIA "Enables libossia network functionnalities" ON)

add_subdirectory(wpn114)

set(WPN114_INCLUDES ${CMAKE_SOURCE_DIR})

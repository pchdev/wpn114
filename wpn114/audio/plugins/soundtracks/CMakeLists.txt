add_definitions(-DWPN_AUDIO_SNDFILE)

set(WPN114_SOUNDTRACKS_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/soundtracks.cpp)

add_library(soundtracks SHARED ${WPN114_SOUNDTRACKS_IMPLEMENTATIONS})

if(WPN_CONTROL_OSSIA)
    add_definitions(-DWPN_CONTROL_OSSIA)
endif()

target_include_directories(soundtracks PUBLIC ${CMAKE_SOURCE_DIR})

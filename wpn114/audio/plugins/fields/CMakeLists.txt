add_definitions(-DWPN_AUDIO_SNDFILE)

set(WPN114_FIELDS_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/fields.cpp)

add_library(fields SHARED ${WPN114_FIELDS_IMPLEMENTATIONS})

if(WPN_OSSIA)
    add_definitions(-DWPN_OSSIA)
endif()

target_include_directories(fields PUBLIC ${CMAKE_SOURCE_DIR})

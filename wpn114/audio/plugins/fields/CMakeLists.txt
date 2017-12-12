add_definitions(-DWPN_AUDIO_SNDFILE)

set(WPN114_FIELDS_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/fields.cpp)

add_library(fields SHARED ${WPN114_FIELDS_IMPLEMENTATIONS})

if(WPN_CONTROL_OSSIA)
    add_definitions(-DWPN_CONTROL_OSSIA)
    if(WPN_CONTROL_OSSIA_TOML)
        add_definitions(-DWPN_CONTROL_OSSIA_TOML)
    endif()
endif()

target_include_directories(fields PUBLIC ${CMAKE_SOURCE_DIR})

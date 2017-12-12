set(WPN114_ROOMS_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/rooms.cpp)

add_library(rooms SHARED ${WPN114_ROOMS_IMPLEMENTATIONS})
target_include_directories(rooms PUBLIC ${CMAKE_SOURCE_DIR})

if(WPN_CONTROL_OSSIA)
    add_definitions(-DWPN_CONTROL_OSSIA)
    if(WPN_CONTROL_OSSIA_TOML)
        add_definitions(-DWPN_CONTROL_OSSIA_TOML)
    endif()
endif()

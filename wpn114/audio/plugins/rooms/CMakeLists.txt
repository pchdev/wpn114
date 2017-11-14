set(WPN114_ROOMS_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/rooms.cpp)

add_library(rooms SHARED ${WPN114_ROOMS_IMPLEMENTATIONS})
target_include_directories(rooms PUBLIC ${CMAKE_SOURCE_DIR})

if(WPN_OSSIA)
    add_definitions(-DWPN_OSSIA)
endif()

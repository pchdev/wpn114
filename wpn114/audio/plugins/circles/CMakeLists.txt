set(WPN114_CIRCLES_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/circles.cpp)

add_library(circles SHARED ${WPN114_CIRCLES_IMPLEMENTATIONS})
target_include_directories(circles PUBLIC ${CMAKE_SOURCE_DIR})

if(WPN_OSSIA)
    add_definitions(-DWPN_OSSIA)
endif()

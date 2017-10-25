set(WPN114_ASH_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/ash.cpp)

add_library(ash SHARED ${WPN114_ASH_IMPLEMENTATIONS})
target_include_directories(ash PUBLIC ${CMAKE_SOURCE_DIR})

if(WPN_OSSIA)
    add_definitions(-DWPN_OSSIA)
endif()

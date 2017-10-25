set(WPN114_AMBISONICS_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/ambisonics.cpp)

add_library(ambisonics SHARED ${WPN114_AMBISONICS_IMPLEMENTATIONS})
target_include_directories(ambisonics PUBLIC ${CMAKE_SOURCE_DIR})

if(WPN_OSSIA)
    add_definitions(-DWPN_OSSIA)
endif()

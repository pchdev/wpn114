set(WPN114_GRANARY_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/granary.cpp)

add_library(granary SHARED ${WPN114_GRANARY_IMPLEMENTATIONS})
target_include_directories(granary PUBLIC ${CMAKE_SOURCE_DIR})

if(WPN_OSSIA)
    add_definitions(-DWPN_OSSIA)
endif()

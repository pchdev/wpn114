set(WPN114_BURSTS_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/bursts.cpp)

add_library(bursts SHARED ${WPN114_BURSTS_IMPLEMENTATIONS})
target_include_directories(bursts PUBLIC ${CMAKE_SOURCE_DIR})

if(WPN_OSSIA)
    add_definitions(-DWPN_OSSIA)
endif()

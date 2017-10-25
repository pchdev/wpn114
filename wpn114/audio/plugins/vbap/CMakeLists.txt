set(WPN114_VBAP_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/vbap.cpp)

add_library(vbap SHARED ${WPN114_VBAP_IMPLEMENTATIONS})
target_include_directories(vbap PUBLIC ${CMAKE_SOURCE_DIR})

if(WPN_OSSIA)
    add_definitions(-DWPN_OSSIA)
endif()

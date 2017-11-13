set(WPN114_ONESHOTS_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/poly-oneshots.cpp)

add_library(oneshotsp SHARED ${WPN114_ONESHOTS_IMPLEMENTATIONS})
target_include_directories(oneshotsp PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(oneshotsp wpn114audio)

if(WPN_OSSIA)
    add_definitions(-DWPN_OSSIA)
endif()

set(WPN114_ONESHOTS_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/oneshots.cpp)

add_library(oneshots SHARED ${WPN114_ONESHOTS_IMPLEMENTATIONS})
target_include_directories(oneshots PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(oneshots wpn114audio)

if(WPN_OSSIA)
    add_definitions(-DWPN_OSSIA)
endif()

set(WPN114_PUSH_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/push_controller.hpp)

set(WPN114_PUSH_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/push_controller.cpp)

add_library(wpn114push SHARED ${WPN114_PUSH_HEADERS} ${WPN114_PUSH_IMPLEMENTATIONS})

target_link_libraries(wpn114push wpn114control)
target_include_directories(wpn114push PUBLIC ${CMAKE_SOURCE_DIR})

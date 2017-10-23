set(WPN114_PUSH_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/push_controller.hpp)

set(WPN114_PUSH_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/push_controller.cpp)

add_library(wpnpush SHARED ${WPN114_PUSH_HEADERS} ${WPN114_PUSH_IMPLEMENTATIONS})
target_include_directories(wpnpush PUBLIC ${CMAKE_SOURCE_DIR})

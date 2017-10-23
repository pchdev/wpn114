set(WPN114_CONTROL_BACKEND_HEADERS
   ${CMAKE_CURRENT_SOURCE_DIR}/backend.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/controller_base.hpp)

set(WPN114_CONTROL_BACKEND_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/backend.cpp)

add_library(wpn114control SHARED
    ${WPN114_CONTROL_BACKEND_HEADERS}
    ${WPN114_CONTROL_BACKEND_IMPLEMENTATIONS})
target_link_libraries(wpn114control /usr/local/opt/rtmidi/lib/librtmidi.a)
target_include_directories(wpn114control PUBLIC ${CMAKE_SOURCE_DIR})

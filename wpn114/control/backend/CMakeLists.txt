set(WPN114_CONTROL_BACKEND_HEADERS
   ${CMAKE_CURRENT_SOURCE_DIR}/backend.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/controller_base.hpp)

set(WPN114_CONTROL_BACKEND_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/backend.cpp)

add_library(wpn114control SHARED
    ${WPN114_CONTROL_BACKEND_HEADERS}
    ${WPN114_CONTROL_BACKEND_IMPLEMENTATIONS})

add_definitions(-D__MACOSX_CORE__)

target_include_directories(wpn114control PRIVATE /usr/local/opt/rtmidi/include)

if(APPLE)
    target_link_libraries(wpn114control "-framework CoreMidi")
    target_link_libraries(wpn114control "-framework CoreAudio")
    target_link_libraries(wpn114control "-framework CoreFoundation")
endif()

target_link_libraries(wpn114control /usr/local/opt/rtmidi/lib/librtmidi.a)
target_include_directories(wpn114control PUBLIC ${CMAKE_SOURCE_DIR})

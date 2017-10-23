set(WPN114_VST_HEADERS
   ${CMAKE_CURRENT_SOURCE_DIR}/vst.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/vstwindow.h)

set(WPN114_VST_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/vst.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vst2x.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/vstwindow.m)

add_library(wpn114vst SHARED
    ${WPN114_VST_HEADERS}
    ${WPN114_VST_IMPLEMENTATIONS})


if(APPLE)
    target_link_libraries(wpn114vst "-framework AppKit")
    target_link_libraries(wpn114vst "-framework Cocoa")
    target_link_libraries(wpn114vst "-framework Foundation")
    target_link_libraries(wpn114vst "-framework CoreFoundation")
endif(APPLE)

add_definitions(-DDEVELOPMENT)
target_include_directories(wpn114vst PUBLIC ${CMAKE_SOURCE_DIR}/dependencies/vst3sdk/pluginterfaces/vst2.x)
target_include_directories(wpn114vst PUBLIC ${CMAKE_SOURCE_DIR})

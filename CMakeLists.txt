project(wpn114)

cmake_minimum_required(VERSION 3.7)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
 )

set(WPN114_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src)

set(WPN114_HDRS    
    ${CMAKE_SOURCE_DIR}/src/wpn114/audio/backend.hpp
    ${CMAKE_SOURCE_DIR}/src/wpn114/audio/context.hpp
    ${CMAKE_SOURCE_DIR}/src/wpn114/audio/soundfiles/soundfile_handler.hpp
    ${CMAKE_SOURCE_DIR}/src/wpn114/audio/units/plugins/vst.hpp
    ${CMAKE_SOURCE_DIR}/src/wpn114/audio/units/plugins/vstwindow.h
    ${CMAKE_SOURCE_DIR}/src/wpn114/audio/units/fields/fields.hpp
    ${CMAKE_SOURCE_DIR}/src/wpn114/audio/units/unit_base.hpp
    ${CMAKE_SOURCE_DIR}/src/wpn114/control/controller_base.hpp
    ${CMAKE_SOURCE_DIR}/src/wpn114/control/midi/backend.hpp
    ${CMAKE_SOURCE_DIR}/src/wpn114/control/midi/push/push_controller.hpp

    )

set(WPN114_SRCS    
    ${CMAKE_SOURCE_DIR}/src/wpn114/audio/backend.cpp
    ${CMAKE_SOURCE_DIR}/src/wpn114/audio/soundfiles/soundfile_handler.cpp
    ${CMAKE_SOURCE_DIR}/src/wpn114/audio/units/plugins/vst.cpp
    ${CMAKE_SOURCE_DIR}/src/wpn114/audio/units/plugins/vst2x.mm
    ${CMAKE_SOURCE_DIR}/src/wpn114/audio/units/plugins/vstwindow.m
    ${CMAKE_SOURCE_DIR}/src/wpn114/audio/units/fields/fields.cpp
    ${CMAKE_SOURCE_DIR}/src/wpn114/control/midi/backend.cpp
    ${CMAKE_SOURCE_DIR}/src/wpn114/control/midi/push/push_controller.cpp
    )

set(WPN114_QML_SRCS
    ${CMAKE_SOURCE_DIR}/view/main.qml
    )

find_package(Sndfile REQUIRED)
find_package(Portaudio REQUIRED)
find_package(Qt5 COMPONENTS Quick Core)
find_package(Qt5QuickCompiler)

#find_library(APP_KIT, AppKit)
#find_library(COCOA, Cocoa)
#find_library(FOUNDATION, Foundation)
#find_library(CORE_FOUNDATION, CoreFoundation)

qt5_add_resources(RESOURCES ${WPN114_QML_SRCS})

add_definitions(-DDEVELOPMENT)

add_library(wpn114 ${WPN114_HDRS} ${WPN114_SRCS})

#add_executable(${PROJECT_NAME} ${WPN114_HDRS} ${WPN114_SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/dependencies/vst3sdk/pluginterfaces/vst2.x)

target_link_libraries(wpn114 "-framework AppKit")
target_link_libraries(wpn114 "-framework Cocoa")
target_link_libraries(wpn114 "-framework Foundation")
target_link_libraries(wpn114 "-framework CoreFoundation")
target_link_libraries(wpn114 ${PORTAUDIO_LIBRARIES})

add_executable(quarre main.cpp)
target_include_directories(quarre PUBLIC ${WPN114_INCLUDE_DIR})
target_link_libraries(quarre wpn114)




qt5_use_modules(${PROJECT_NAME} Quick Core)


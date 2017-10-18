project(wpn114)

cmake_minimum_required(VERSION 3.7)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
 )

set(WPN114_HDRS
    ${CMAKE_SOURCE_DIR}/src/audio/backend/audio_backend.hpp
    ${CMAKE_SOURCE_DIR}/src/audio/backend/soundfiles/soundfile_handler.hpp
    ${CMAKE_SOURCE_DIR}/src/audio/units/vst/vst.hpp
    ${CMAKE_SOURCE_DIR}/src/audio/units/units_base.hpp
    ${CMAKE_SOURCE_DIR}/src/control/controller_base.hpp
    ${CMAKE_SOURCE_DIR}/src/control/midi/backend/midi_backend.hpp
    )

set(WPN114_SRCS
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/src/audio/backend/audio_backend.cpp
    ${CMAKE_SOURCE_DIR}/src/audio/backend/soundfiles/soundfile_handler.cpp
    ${CMAKE_SOURCE_DIR}/src/audio/units/vst/vst.cpp
    ${CMAKE_SOURCE_DIR}/src/audio/units/vst/vst2x.mm
    ${CMAKE_SOURCE_DIR}/src/control/midi/backend/midi_backend.cpp
    )

set(WPN114_QML_SRCS
    ${CMAKE_SOURCE_DIR}/view/main.qml
    )

find_package(Sndfile REQUIRED)
find_package(Portaudio REQUIRED)
find_package(Qt5 COMPONENTS Quick Core)
find_package(Qt5QuickCompiler)

qt5_add_resources(RESOURCES ${WPN114_QML_SRCS})

add_definitions(-DDEVELOPMENT)

add_executable(${PROJECT_NAME} ${WPN114_HDRS} ${WPN114_SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/dependencies/vst3sdk/pluginterfaces/vst2.x)

qt5_use_modules(${PROJECT_NAME} Quick Core)


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
    ${CMAKE_SOURCE_DIR}/src/audio/backend/backend.h
    ${CMAKE_SOURCE_DIR}/src/audio/sndfile/sndf.h
    ${CMAKE_SOURCE_DIR}/src/audio/vst/vst.h
    )

set(WPN114_SRCS
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/src/audio/backend/backend.cpp
    ${CMAKE_SOURCE_DIR}/src/audio/sndfile/sndf.cpp
    ${CMAKE_SOURCE_DIR}/src/audio/vst/vst.cpp
    ${CMAKE_SOURCE_DIR}/src/audio/vst/vst2x.mm
    )

find_package(Sndfile REQUIRED)
find_package(Portaudio REQUIRED)
find_package(Qt5 COMPONENTS Quick Core)
find_package(Qt5QuickCompiler)

add_definitions(-DDEVELOPMENT)

add_executable(${PROJECT_NAME} ${WPN114_HDRS} ${WPN114_SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/dependencies/vst3sdk/pluginterfaces/vst2.x)

qt5_use_modules(${PROJECT_NAME} Quick Core)

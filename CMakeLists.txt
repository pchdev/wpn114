project(wpn114)

cmake_minimum_required(VERSION 3.7)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
 )

set(WPN114_HDRS)
set(WPN114_SRCS)

find_package(Sndfile REQUIRED)
find_package(Portaudio REQUIRED)

add_executable(${PROJECT_NAME} "main.cpp")


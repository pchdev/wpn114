find_package(Portaudio REQUIRED)

set(WPN114_AUDIO_BACKEND_HEADERS
   ${CMAKE_CURRENT_SOURCE_DIR}/backend.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/context.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/unit_base.hpp)

set(WPN114_AUDIO_BACKEND_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/backend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit_base.cpp)

if(WPN_OSSIA)
    add_definitions(-DWPN_OSSIA)
endif()

if(WPN_AUDIO_AUX)
    add_definitions(-DWPN_AUDIO_AUX)
endif()

if(WPN_AUDIO_SNDFILE)
    add_definitions(-DWPN_AUDIO_SNDFILE)
    find_package(Sndfile REQUIRED)
    set(WPN114_AUDIO_BACKEND_HEADERS
        ${WPN114_AUDIO_BACKEND_HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/sndfile_support.hpp)
    set(WPN114_AUDIO_BACKEND_IMPLEMENTATIONS
        ${WPN114_AUDIO_BACKEND_IMPLEMENTATIONS}
        ${CMAKE_CURRENT_SOURCE_DIR}/sndfile_support.cpp)
endif()

if(WPN_AUDIO_TRACKS)
    add_definitions(-DWPN_AUDIO_TRACKS)
endif()

add_library(wpn114audio SHARED
    ${WPN114_AUDIO_BACKEND_HEADERS}
    ${WPN114_AUDIO_BACKEND_IMPLEMENTATIONS})

target_include_directories(wpn114audio PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(wpn114audio ${PORTAUDIO_LIBRARIES})
target_link_libraries(wpn114audio ${SNDFILE_LIBRARIES})



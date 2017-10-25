find_package(Sndfile REQUIRED)
find_package(Portaudio REQUIRED)

set(WPN114_AUDIO_BACKEND_HEADERS
   ${CMAKE_CURRENT_SOURCE_DIR}/backend.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/context.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/sndfile_support.hpp
   ${CMAKE_CURRENT_SOURCE_DIR}/unit_base.hpp)

set(WPN114_AUDIO_BACKEND_IMPLEMENTATIONS
    ${CMAKE_CURRENT_SOURCE_DIR}/backend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sndfile_support.cpp)

add_library(wpn114audio SHARED
    ${WPN114_AUDIO_BACKEND_HEADERS}
    ${WPN114_AUDIO_BACKEND_IMPLEMENTATIONS})

target_include_directories(wpn114audio PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(wpn114audio ${PORTAUDIO_LIBRARIES})
target_link_libraries(wpn114audio ${SNDFILE_LIBRARIES})

if(WPN_OSSIA)
    add_definitions(-DWPN_OSSIA)
    #target_include_directories(wpn114audio PUBLIC /usr/local/include)
    #target_link_libraries(wpn114audio /usr/local/lib/libossia.dylib)
endif()



add_definitions(-DWPN114_VERSION_STRING="CRNA17004017")

set(CPPTOML_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/cpptoml)
set(WPN_EXEC ${CMAKE_SOURCE_DIR}/main.cpp)

if(WPN_TOML_PARSING)
    list(APPEND WPN_EXEC ${CMAKE_SOURCE_DIR}/main.toml)
endif()

add_executable(${PROJECT_NAME} ${WPN_EXEC})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

if(WPN_AUDIO)

    message("WPN114: Building ${PROJECT_NAME} with audio features")
    add_subdirectory(audio)
    target_link_libraries(${PROJECT_NAME} wpn114audio)

    if(WPN_AUDIO_SNDFILE)
        message("WPN114: building with sndfile library")
        add_definitions(-DWPN_AUDIO_SNDFILE)
    endif()

    if(WPN_AUDIO_FFMPEG)
        message("WPN114: building with ffmpeg library")
        add_definitions(-DWPN_AUDIO_FFMPEG)
    endif()

    if(WPN_AUDIO_AUX)
        message("WPN114: building with aux features")
        add_definitions(-DWPN_AUDIO_AUX)
    endif()

    if(WPN_AUDIO_TRACKS)
        message("WPN114: building with tracks features")
        add_definitions(-DWPN_AUDIO_TRACKS)
    endif()

    if(WPN_AUDIO_PLUGINS_AMBISONICS)
        message("WPN114: building with ambisonics audio plugin")
        target_link_libraries(${PROJECT_NAME} ambisonics)
    endif()

    if(WPN_AUDIO_PLUGINS_ASH)
        message("WPN114: building with ash audio plugin")
        target_link_libraries(${PROJECT_NAME} ash)
    endif()

    if(WPN_AUDIO_PLUGINS_BURSTS)
        message("WPN114: building with burst audio plugin")
        target_link_libraries(${PROJECT_NAME} bursts)
    endif()

    if(WPN_AUDIO_PLUGINS_FIELDS)
        message("WPN114: building with fields audio plugin")
        target_link_libraries(${PROJECT_NAME} fields)
    endif()

    if(WPN_AUDIO_PLUGINS_GRANARY)
        message("WPN114: building with granary audio plugin")
        target_link_libraries(${PROJECT_NAME} granary)
    endif()

    if(WPN_AUDIO_PLUGINS_ONESHOTS)
        message("WPN114: building with oneshots audio plugin")
        target_link_libraries(${PROJECT_NAME} oneshots)
    endif()

    if(WPN_AUDIO_PLUGINS_ROOMS)
        message("WPN114: building with rooms audio plugin")
        target_link_libraries(${PROJECT_NAME} rooms)
    endif()

    if(WPN_AUDIO_PLUGINS_VBAP)
        message("WPN114: building with vbap audio plugin")
        target_link_libraries(${PROJECT_NAME} vbap)
    endif()

    if(WPN_AUDIO_PLUGINS_VST)
        message("WPN114: building with vst audio support")
        target_link_libraries(${PROJECT_NAME} wpn114vst)
    endif()

else()
    message("WPN114: Building ${PROJECT_NAME} without audio features")
endif()

if(WPN_CONTROL)

    message("WPN114: Building ${PROJECT_NAME} with control features")

    if(WPN_CONTROL_MIDI) #to update with other protocols
        add_subdirectory(control)
        target_link_libraries(${PROJECT_NAME} wpn114control)
    endif()

    if(WPN_CONTROL_OSSIA)
        message("WPN114: Building ${PROJECT_NAME} with libossia features")
        set(OSSIA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/libossia/OSSIA)
        target_include_directories(${PROJECT_NAME} PUBLIC ${OSSIA_INCLUDE_DIR})
        add_definitions(-DWPN_CONTROL_OSSIA)
        add_subdirectory(network)
        target_link_libraries(${PROJECT_NAME} ossia)
        target_link_libraries(${PROJECT_NAME} wpn114net)
    endif()

    if(WPN_CONTROL_COMMAND_LINE)
        message("WPN114: Building ${PROJECT_NAME} with command-line features")
        add_subdirectory(command)
        #target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}command)
    else()
        message("WPN114: Building ${PROJECT_NAME} without command-line features")
    endif()

    if(WPN_CONTROL_PLUGINS_PUSH1)
        message("WPN114: building with ableton push control plugin")
        target_link_libraries(${PROJECT_NAME} wpn114push)
    endif()

else()
    message("WPN114: Building ${PROJECT_NAME} without control features")
endif()

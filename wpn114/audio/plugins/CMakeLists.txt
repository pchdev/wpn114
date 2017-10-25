
if(WPN_AUDIO_PLUGINS_SFPLAY AND WPN_AUDIO)
endif()

if(WPN_AUDIO_PLUGINS_AMBISONICS AND WPN_AUDIO)
    add_subdirectory(ambisonics)
endif()

if(WPN_AUDIO_PLUGINS_ASH AND WPN_AUDIO)
    add_subdirectory(ash)
endif()

if(WPN_AUDIO_PLUGINS_BURSTS AND WPN_AUDIO)
    add_subdirectory(bursts)
endif()

if(WPN_AUDIO_PLUGINS_FIELDS AND WPN_AUDIO)
    add_subdirectory(fields)
endif()

if(WPN_AUDIO_PLUGINS_GRANARY AND WPN_AUDIO)
    add_subdirectory(granary)
endif()

if(WPN_AUDIO_PLUGINS_VBAP AND WPN_AUDIO)
    add_subdirectory(vbap)
endif()

if(WPN_AUDIO_PLUGINS_VST AND WPN_AUDIO)
    add_subdirectory(vst)
endif()

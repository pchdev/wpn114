if(WPN_AUDIO_PLUGINS_VST AND WPN_AUDIO)
    add_subdirectory(vst)
endif()

if(WPN_AUDIO_PLUGINS_FIELDS AND WPN_AUDIO)
    add_subdirectory(fields)
endif()

if(WPN_AUDIO_PLUGINS_ASHES AND WPN_AUDIO)
endif()

if(WPN_AUDIO_PLUGINS_SFPLAY AND WPN_AUDIO)
endif()


if(WPN_AUDIO_PLUGINS_AMBISONICS)
    add_subdirectory(ambisonics)
endif()

if(WPN_AUDIO_PLUGINS_ASH)
    add_subdirectory(ash)
endif()

if(WPN_AUDIO_PLUGINS_BURSTS)
    add_subdirectory(bursts)
endif()

if(WPN_AUDIO_PLUGINS_CIRCLES)
    add_subdirectory(circles)
endif()

if(WPN_AUDIO_PLUGINS_FIELDS)
    add_subdirectory(fields)
endif()

if(WPN_AUDIO_PLUGINS_GRANARY)
    add_subdirectory(granary)
endif()

if(WPN_AUDIO_PLUGINS_ONESHOTS)
    add_subdirectory(oneshots)
endif()

if(WPN_AUDIO_PLUGINS_POLY_ONESHOTS)
    add_subdirectory(poly-oneshots)
endif()

if(WPN_AUDIO_PLUGINS_ROOMS)
    add_subdirectory(rooms)
endif()

if(WPN_AUDIO_PLUGINS_VBAP)
    add_subdirectory(vbap)
endif()

if(WPN_AUDIO_PLUGINS_VST)
    add_subdirectory(vst)
endif()

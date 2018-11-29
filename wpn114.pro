TARGET = WPN114
TEMPLATE = lib
CONFIG += c++11 dll
QT += quick

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QZEROCONF_STATIC

android {
    CONFIG += network
    DESTDIR = /Users/pchd/Qt/5.11.1/android_armv7/qml/WPN114
}

macx {
    QMAKE_MAC_SDK = macosx10.14
    CONFIG += audio midi network vst
    LIBS +=  \
    -framework CoreFoundation \
    -framework CoreAudio \
    -framework CoreMIDI    
    DESTDIR = /Users/pchd/Qt/5.11.1/clang_64/qml/WPN114
    DEFINES += __MACOSX_CORE__
}

linux {
    CONFIG += audio midi network
    DESTDIR = /usr/lib/x86_64-linux-gnu/qt5/qml/WPN114
}

include ( external/qtzeroconf/qtzeroconf.pri )

vst {
    DEFINES += WPN114_VST
    QT += widgets
    HEADERS += audio_objects/audioplugin/aeffect.h \
    audio_objects/audioplugin/aeffectx.h \
    audio_objects/audioplugin/audioplugin.hpp
    SOURCES += audio_objects/audioplugin/audioplugin.mm
}

audio {
    DEFINES += WPN114_AUDIO
    SOURCES +=                                      \
        source/audio/audio.cpp                      \
        external/rtaudio/RtAudio.cpp                \
        audio_objects/sine/sine.cpp                 \
        audio_objects/stpanner/stereopanner.cpp     \
        source/audio/soundfile.cpp                     \
        audio_objects/sampler/sampler.cpp           \
        audio_objects/rooms/rooms.cpp               \
        audio_objects/mangler/mangler.cpp           \
        audio_objects/sharpen/sharpen.cpp           \
        audio_objects/multisampler/multisampler.cpp \
        audio_objects/fork/fork.cpp                 \
        audio_objects/peakrms/peakrms.cpp           \
        audio_objects/convolver/convolver.cpp       \
        external/fftconvolver/AudioFFT.cpp        \
        external/fftconvolver/FFTConvolver.cpp    \
        external/fftconvolver/TwoStageFFTConvolver.cpp \
        external/fftconvolver/Utilities.cpp       \
        audio_objects/clock/audioclock.cpp          \
        audio_objects/bursts/bursts.cpp             \
        audio_objects/limiter/masterlimiter.cpp     \
        audio_objects/ashes/ashes.cpp               \
        audio_objects/downmix/downmix.cpp           \
        audio_objects/channelmapper/channelmapper.cpp \
        audio_objects/hlpf/filter.cpp

    HEADERS +=                                      \
        source/audio/audio.hpp                      \
        source/audio/soundfile.hpp                  \
        external/rtaudio/RtAudio.h                  \
        audio_objects/sine/sine.hpp                 \
        audio_objects/stpanner/stereopanner.hpp     \
        audio_objects/sampler/sampler.hpp           \
        audio_objects/rooms/rooms.hpp               \
        audio_objects/mangler/mangler.hpp           \
        audio_objects/sharpen/sharpen.hpp           \
        audio_objects/multisampler/multisampler.hpp \
        audio_objects/fork/fork.hpp                 \
        audio_objects/peakrms/peakrms.hpp           \
        audio_objects/convolver/convolver.hpp       \
        external/fftconvolver/AudioFFT.h          \
        external/fftconvolver/FFTConvolver.h      \
        external/fftconvolver/TwoStageFFTConvolver.h \
        external/fftconvolver/Utilities.h         \
        audio_objects/clock/audioclock.hpp          \
        audio_objects/bursts/bursts.hpp             \
        audio_objects/limiter/masterlimiter.hpp     \
        audio_objects/ashes/ashes.hpp               \
        audio_objects/downmix/downmix.hpp           \
        audio_objects/channelmapper/channelmapper.hpp \
        audio_objects/hlpf/filter.hpp
}

midi {
    DEFINES += WPN114_MIDI
    HEADERS += source/midi/midi.hpp
    SOURCES += source/midi/midi.cpp
    HEADERS += external/rtmidi/RtMidi.h
    SOURCES += external/rtmidi/RtMidi.cpp
    HEADERS += source/midi/devices/push/device_enums.hpp
}

network {
    QT += network
    DEFINES += WPN114_NETWORK
    SOURCES +=                                  \
        source/http/http.cpp                       \
        source/osc/osc.cpp                         \
        source/oscquery/client.cpp                 \
        source/oscquery/device.cpp                 \
        source/oscquery/file.cpp                   \
        source/oscquery/folder.cpp                 \
        source/oscquery/node.cpp                   \
        source/oscquery/server.cpp           \
        source/websocket/websocket.cpp             \
        source/oscquery/tree.cpp               \
        source/oscquery/netexplorer.cpp
    HEADERS +=                                  \
        source/http/http.hpp                       \
        source/osc/osc.hpp                         \
        source/oscquery/client.hpp                 \
        source/oscquery/device.hpp                 \
        source/oscquery/file.hpp                   \
        source/oscquery/folder.hpp                 \
        source/oscquery/node.hpp                   \
        source/oscquery/server.hpp           \
        source/websocket/websocket.hpp             \
        source/oscquery/tree.hpp               \
        source/oscquery/netexplorer.hpp
}

SOURCES += qml_plugin.cpp
HEADERS += qml_plugin.hpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

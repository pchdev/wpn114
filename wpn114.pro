TARGET = WPN114

QT += quick multimedia widgets
TEMPLATE = lib
CONFIG += c++11 dll

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
    #QT += multimedia
    INCLUDEPATH += /usr/local/opt/portaudio/include
    LIBS += -L/usr/local/opt/portaudio/lib/ -lportaudio
    DEFINES += WPN114_AUDIO
    SOURCES +=                                      \
        src/audio/audio.cpp                         \
        audio_objects/sine/sine.cpp                 \
        audio_objects/stpanner/stereopanner.cpp     \
        src/audio/soundfile.cpp                     \
        audio_objects/sampler/sampler.cpp           \
        audio_objects/rooms/rooms.cpp               \
        audio_objects/mangler/mangler.cpp           \
        audio_objects/sharpen/sharpen.cpp           \
        audio_objects/multisampler/multisampler.cpp \
        audio_objects/fork/fork.cpp                 \
        audio_objects/peakrms/peakrms.cpp           \
        audio_objects/convolver/convolver.cpp       \
        audio_objects/convolver/AudioFFT.cpp        \
        audio_objects/convolver/FFTConvolver.cpp    \
        audio_objects/convolver/TwoStageFFTConvolver.cpp \
        audio_objects/convolver/Utilities.cpp       \
        audio_objects/clock/audioclock.cpp          \
        audio_objects/bursts/bursts.cpp             \
        audio_objects/limiter/masterlimiter.cpp     \
        audio_objects/ashes/ashes.cpp               \
        audio_objects/downmix/downmix.cpp           \
        audio_objects/channelmapper/channelmapper.cpp

    HEADERS +=                                      \
        src/audio/audio.hpp                         \
        audio_objects/sine/sine.hpp                 \
        audio_objects/stpanner/stereopanner.hpp     \
        src/audio/soundfile.hpp                     \
        audio_objects/sampler/sampler.hpp           \
        audio_objects/rooms/rooms.hpp               \
        audio_objects/mangler/mangler.hpp           \
        audio_objects/sharpen/sharpen.hpp           \
        audio_objects/multisampler/multisampler.hpp \
        audio_objects/fork/fork.hpp                 \
        audio_objects/peakrms/peakrms.hpp           \
        audio_objects/convolver/convolver.hpp       \
        audio_objects/convolver/AudioFFT.h          \
        audio_objects/convolver/FFTConvolver.h      \
        audio_objects/convolver/TwoStageFFTConvolver.h \
        audio_objects/convolver/Utilities.h         \
        audio_objects/clock/audioclock.hpp          \
        audio_objects/bursts/bursts.hpp             \
        audio_objects/limiter/masterlimiter.hpp     \
        audio_objects/ashes/ashes.hpp               \
        audio_objects/downmix/downmix.hpp           \
        audio_objects/channelmapper/channelmapper.hpp
}

midi {
    DEFINES += WPN114_MIDI
    DEFINES += __MACOSX_CORE__
    HEADERS += src/midi/midi.hpp
    SOURCES += src/midi/midi.cpp
    HEADERS += src/midi/RtMidi.h
    SOURCES += src/midi/RtMidi.cpp
    HEADERS += src/midi/devices/push/device_enums.hpp
}

network {
    DEFINES += WPN114_NETWORK
    SOURCES +=                                  \
        src/http/http.cpp                       \
        src/osc/osc.cpp                         \
        src/oscquery/client.cpp                 \
        src/oscquery/device.cpp                 \
        src/oscquery/file.cpp                   \
        src/oscquery/folder.cpp                 \
        src/oscquery/node.cpp                   \
        src/oscquery/query-server.cpp           \
        src/websocket/websocket.cpp             \
        src/oscquery/nodetree.cpp               \
        src/oscquery/netexplorer.cpp
    HEADERS +=                                  \
        src/http/http.hpp                       \
        src/osc/osc.hpp                         \
        src/oscquery/client.hpp                 \
        src/oscquery/device.hpp                 \
        src/oscquery/file.hpp                   \
        src/oscquery/folder.hpp                 \
        src/oscquery/node.hpp                   \
        src/oscquery/query-server.hpp           \
        src/websocket/websocket.hpp             \
        src/oscquery/nodetree.hpp               \
        src/oscquery/netexplorer.hpp
}

SOURCES += qml_plugin.cpp
HEADERS += qml_plugin.hpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

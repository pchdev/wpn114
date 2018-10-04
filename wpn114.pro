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
DEFINES += "ANDROID_JSON=0"

android {
    DESTDIR = /Users/pchd/Qt/5.11.1/android_armv7/qml/WPN114
    DEFINES += "ANDROID_JSON=1"
}

macx {
    QMAKE_MAC_SDK = macosx10.14
    LIBS +=  \
    -framework CoreFoundation \
    -framework CoreAudio \
    -framework CoreMIDI

    DESTDIR = /Users/pchd/Qt/5.11.1/clang_64/qml/WPN114
}

include ( external/qtzeroconf/qtzeroconf.pri )

SOURCES +=                      \
qml_plugin.cpp                  \
src/audio/audio.cpp             \
audio_objects/sine/sine.cpp     \
src/midi/midi.cpp               \
src/midi/RtMidi.cpp             \
src/http/http.cpp               \
src/osc/osc.cpp                 \
src/oscquery/client.cpp         \
src/oscquery/device.cpp         \
src/oscquery/file.cpp           \
src/oscquery/folder.cpp         \
src/oscquery/node.cpp           \
src/oscquery/query-server.cpp   \
src/websocket/websocket.cpp \
    audio_objects/stpanner/stereopanner.cpp \
    src/audio/soundfile.cpp \
    audio_objects/sampler/sampler.cpp \
    audio_objects/audioplugin/audioplugin.mm \
    audio_objects/rooms/rooms.cpp \
    audio_objects/mangler/mangler.cpp \
    audio_objects/sharpen/sharpen.cpp

HEADERS +=                      \
qml_plugin.hpp                  \
src/audio/audio.hpp             \
audio_objects/sine/sine.hpp     \
src/midi/midi.hpp               \
src/midi/RtMidi.h               \
src/http/http.hpp               \
src/osc/osc.hpp                 \
src/oscquery/client.hpp         \
src/oscquery/device.hpp         \
src/oscquery/file.hpp           \
src/oscquery/folder.hpp         \
src/oscquery/node.hpp           \
src/oscquery/query-server.hpp   \
src/websocket/websocket.hpp \
    audio_objects/stpanner/stereopanner.hpp \
    src/audio/soundfile.hpp \
    audio_objects/sampler/sampler.hpp \
    audio_objects/audioplugin/aeffect.h \
    audio_objects/audioplugin/aeffectx.h \
    audio_objects/audioplugin/audioplugin.hpp \
    audio_objects/rooms/rooms.hpp \
    audio_objects/mangler/mangler.hpp \
    audio_objects/sharpen/sharpen.hpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#include <iostream>
#include "src/audio/backend/audio_backend.hpp"
#include "src/audio/units/vst/vst.hpp"
#include <QGuiApplication>
#include <QQmlApplicationEngine>

using namespace std;

// a vst host for kaivo
// vst host for absynth
// vst host for altiverb

// custom granular / multifunction sampler
// libossia for communication
// ableton push support?

// dependencies:
// - vst3sdk
// - libossia
// - portaudio
// - libsndfile

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 256

int main()
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if(engine.rootObjects().isEmpty()) return -1;

    wpn114::audio::vst::plugin_handler kaivo_1("Kaivo");
    //wpn114::audio::vst::plugin_handler absynth_1("Absynth");
    //wpn114::audio::vst::plugin_handler altiverb("Altiverb");
    //wpn114::audio::vst::plugin_handler amplitube("Amplitube");

    //wpn114::audio::units::fields("/path/to/soundfile.wav");

    //auto& stream = wpn114::audio::backend::initialize_audio(SAMPLE_RATE, FRAMES_PER_BUFFER);
    // init controller
    // init view (qml or command line)

    return app.exec();
}

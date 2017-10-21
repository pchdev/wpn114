#include <iostream>

#include <wpn114/audio/backend.hpp>
#include <wpn114/audio/units/plugins/vst.hpp>
#include <wpn114/audio/context.hpp>

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

// vst take float** inputs and outputs
//

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 256

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if(engine.rootObjects().isEmpty()) return -1;

    wpn114::audio::context.blocksize = 256;
    wpn114::audio::context.sample_rate = 44100;
    wpn114::audio::context.num_inputs = 0;
    wpn114::audio::context.num_outputs = 2;

    wpn114::audio::backend::backend backend(2);

    wpn114::audio::units::plugin_handler kaivo_1("Kaivo");
    //wpn114::audio::vst::plugin_handler absynth_1("Absynth");
    //wpn114::audio::vst::plugin_handler altiverb("Altiverb");
    //wpn114::audio::vst::plugin_handler amplitube("Amplitube");

    //wpn114::audio::units::fields("/path/to/soundfile.wav");

    //auto& stream = wpn114::audio::backend::initialize_audio(SAMPLE_RATE, FRAMES_PER_BUFFER);
    // init controller
    // init view (qml or command line)

    return app.exec();
}

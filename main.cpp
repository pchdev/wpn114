#include <iostream>
#include "src/audio/backend/backend.h"
#include "src/audio/vst/vst.h"
#include <QCoreApplication>

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
    std::string name("Kaivo");
    wpn114::audio::vst::load_vst(name);
    //wpn114::audio::vst::load_vst("/Library/Audio/Plug-Ins/VST/Absynth.vst");
    //wpn114::audio::vst::load_vst("/Library/Audio/Plug-Ins/VST/Altiverb.vst");
    //wpn114::audio::units::fields("/path/to/soundfile.wav");

    //auto& stream = wpn114::audio::backend::initialize_audio(SAMPLE_RATE, FRAMES_PER_BUFFER);
    // init controller
    // init view (qml)


    // loop

    return 0;
}

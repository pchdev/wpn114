#include <wpn114/audio/backend/backend.hpp>
#include <wpn114/audio/backend/context.hpp>
#include <wpn114/audio/plugins/vst/vst.hpp>
#include <wpn114/audio/plugins/fields/fields.cpp>

#include <wpn114/control/plugins/push_1/push_controller.hpp>
#include <iostream>

using namespace wpn114;

int main(int argc, char* argv[])
{
    wpn114::audio::context.blocksize = 256;
    wpn114::audio::context.sample_rate = 44100;
    wpn114::audio::context.num_inputs = 0;
    wpn114::audio::context.num_outputs = 2;
    wpn114::audio::context.master_tempo = 120.f;

    //wpn114::audio::backend audio_backend(2);

    //wpn114::audio::units::plugin_handler kaivo_1("Kaivo.vst");
    //wpn114::audio::units::plugin_handler absynth_1("Absynth.vst");
    //wpn114::audio::units::plugin_handler altiverb("Altiverb.vst");
    //wpn114::audio::units::plugin_handler amplitube("Amplitube.vst");

    wpn114::audio::plugins::fields sf_1("/path/to/soundfile.wav");

    wpn114::audio::backend_hdl audio_hdl(2);
    audio_hdl.register_unit(&sf_1);
    audio_hdl.initialize();
    audio_hdl.start_stream();

    // init controller
    /*wpn114::control::midi::device_factory push_device_factory;
    std::string push_port_name = "Ableton Push User Port";

    auto push_hdl = push_device_factory.make_device_hdl(push_port_name,
                                 wpn114::control::device_io_type::IN_OUT);

    wpn114::control::midi::push_controller push(std::move(push_hdl));

    // init view (qml or command line)*/

    return 0;
}

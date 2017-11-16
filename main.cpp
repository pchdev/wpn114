#include <wpn114/audio/backend/backend.hpp>
#include <wpn114/audio/backend/context.hpp>
#include <wpn114/audio/plugins/vst/vst.hpp>
#include <wpn114/audio/plugins/fields/fields.cpp>
#include <wpn114/audio/plugins/oneshots/oneshots.cpp>
//#include <wpn114/control/plugins/push_1/push_controller.hpp>
#include <wpn114/network/net_hdl.hpp>
#include <iostream>
#include <time.h>

using namespace wpn114;

#define BLOCKSIZE 512
#define SAMPLERATE 44100

int main(int argc, char* argv[])
{
    net::net_hdl net_hdl("quarre-audio");
    net_hdl.expose_oscquery_server(1234, 5678);

    audio::backend_hdl audio_hdl(2);

    audio::plugins::oneshots os_test("test.wav");
    os_test.net_expose(net_hdl.get_application_node(), "os_test");
    audio_hdl.register_unit(&os_test);

    audio::aux_unit bus_1();

    audio::plugins::vst_hdl kaivo_1("Kaivo.vst");
    audio_hdl.register_unit(&kaivo_1);
    kaivo_1.net_expose(net_hdl.get_application_node(), "kaivo_1");
    kaivo_1.add_aux_send(bus_1);

    audio::plugins::vst_hdl kaivo_2("Kaivo.vst");
    kaivo_2.net_expose(net_hdl.get_application_node(), "kaivo_2");

    audio::plugins::vst_hdl altiverb("Audio Ease/Altiverb 7.vst");
    altiverb.net_expose(net_hdl.get_application_node(), "altiverb");
    bus_1.set_receiver(std::make_unique<audio::unit_base>(altiverb));

    audio::plugins::fields fields_test("test.wav", 32768);
    audio_hdl.register_unit(&fields_test);
    fields_test.net_expose(net_hdl.get_application_node(), "fields");

    audio_hdl.initialize(SAMPLERATE, BLOCKSIZE);
    audio_hdl.start_stream(SAMPLERATE, BLOCKSIZE);

    kaivo_1.show_editor();

    // init controller
    /*wpn114::control::midi::device_factory push_device_factory;
    std::string push_port_name = "Ableton Push User Port";

    auto push_hdl = push_device_factory.make_device_hdl(push_port_name,
                                 wpn114::control::device_io_type::IN_OUT);

    wpn114::control::midi::push_controller push(std::move(push_hdl));

    // init view (qml or command line)*/

    //while(true)
      //  ;

    std::this_thread::sleep_for(std::chrono::milliseconds(20000));

    audio_hdl.stop_stream();

    return 0;
}

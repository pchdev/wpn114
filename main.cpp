#include <wpn114/audio/backend/backend.hpp>
#include <wpn114/audio/plugins/vst/vst.hpp>
#include <wpn114/audio/plugins/fields/fields.cpp>
#include <wpn114/audio/plugins/oneshots/oneshots.cpp>
//#include <wpn114/control/plugins/push_1/push_controller.hpp>
#include <wpn114/network/net_hdl.hpp>
#include <iostream>
#include <time.h>

using namespace wpn114;
using namespace ossia::net;
//-------------------------------------------------------------------------------------------------------
#define BLOCKSIZE 512
#define SAMPLERATE 44100
//-------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // initialize network
    net::net_hdl net_hdl("quarre-audio");
    net_hdl.expose_oscquery_server(1234, 5678);    
    node_base& appnode = net_hdl.get_application_node();

    // initialize audio backend
    audio::backend_hdl audio_hdl(2);

    // instantiate plugins
    audio::plugins::oneshots os_test("test.wav");
        os_test.net_expose(appnode, "os_test");
    audio_hdl.register_unit(&os_test);

    audio::aux_unit bus_1;

    audio::plugins::vst_hdl kaivo_1("Kaivo.vst");
    audio_hdl.register_unit(&kaivo_1);
    kaivo_1.net_expose(appnode, "kaivo_1");
    kaivo_1.add_aux_send(bus_1);

    audio::plugins::vst_hdl kaivo_2("Kaivo.vst");
    kaivo_2.net_expose(appnode, "kaivo_2");

    auto altiverb = std::make_unique<audio::plugins::vst_hdl>("Audio Ease/Altiverb 7.vst");
    altiverb->net_expose(appnode, "altiverb");
    bus_1.set_receiver(std::move(altiverb));

    audio::plugins::fields fields_test("test.wav", 32768);
    audio_hdl.register_unit(&fields_test);
    fields_test.net_expose(appnode, "fields");

    // start audio
    audio_hdl.initialize(   SAMPLERATE, BLOCKSIZE);
    audio_hdl.start_stream( SAMPLERATE, BLOCKSIZE);

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

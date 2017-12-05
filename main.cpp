#include <wpn114/audio/backend/backend.hpp>
#include <wpn114/audio/plugins/vst/vst.hpp>
#include <wpn114/audio/plugins/fields/fields.cpp>
#include <wpn114/audio/plugins/oneshots/oneshots.cpp>
#include <wpn114/audio/plugins/rooms/rooms.cpp>
//#include <wpn114/control/plugins/push_1/push_controller.hpp>
#include <wpn114/network/net_hdl.hpp>
#include <iostream>
#include <time.h>
//-------------------------------------------------------------------------------------------------------
using namespace wpn114;
using namespace ossia::net;
//-------------------------------------------------------------------------------------------------------
#define BLOCKSIZE   512
#define SAMPLERATE  44100
//-------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    std::cout << "WPN114_HASH_"
              << WPN114_VERSION_STRING
              << std::endl;

    // initialize network
    net::net_hdl net_hdl("quarre-audio");
    net_hdl.expose_oscquery_server(1234, 5678);    
    node_base& appnode = net_hdl.application_node();

    // initialize audio backend
    audio::backend_hdl audio_hdl(2);

    // instantiate plugins
    audio::plugins::vst_hdl kaivo_1("Kaivo.vst");
    audio_hdl.register_unit(kaivo_1);

    // start audio
    audio_hdl.initialize(SAMPLERATE, BLOCKSIZE);
    audio_hdl.start(SAMPLERATE, BLOCKSIZE);

    kaivo_1.show_editor();

    //std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    //kaivo_1.close_editor();

    // sleep
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    audio_hdl.stop();

    return 0;
}

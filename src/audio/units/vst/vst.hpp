/*
 * =====================================================================================
 *
 *       Filename:  vst.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  15.10.2017 17:46:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <string>
#include "aeffect.h"
#include "aeffectx.h"
#include "../units_base.hpp"

using aeffect = AEffect;
using vstint32_t = VstInt32;
using vstint64_t = VstInt64;
using vstintptr_t = VstIntPtr;
using vstevents = VstEvents;

extern "C" {

vstintptr_t VSTCALLBACK
host_callback(aeffect* effect, vstint32_t opcode, vstint32_t index,
              vstint32_t value, void *ptr, float opt);

}

typedef aeffect *(*vst_plugin_funcptr)(audioMasterCallback host);

typedef vstintptr_t
(*dispatcher_funcptr)(aeffect* effect, vstint32_t opcode, vstint32_t index,
                      vstint32_t value, void *ptr, float opt);

typedef float (*get_parameter_funcptr)(aeffect* effect, vstint32_t index);
typedef void (*set_parameter_funcptr)(aeffect* effect, vstint32_t index, float value);
typedef vstint32_t (*process_events_funcptr) (vstevents *events);
typedef void (*process_funcptr) (aeffect* effect, float **inputs, float **outputs, vstint32_t sample_frames);

namespace wpn114 {
namespace audio {
namespace vst {

class plugin_handler : public wpn114::audio::units::units_base
{
public:
    plugin_handler(const char* name);
    ~plugin_handler();
    void start();
    void suspend();
    void resume();
    void expose();
    void show();

    void process_audio(float **inputs, float **outputs, long num_frames);
    void silence_channel(float **channel_data, int num_channels, long num_frames);
    void process_midi(vstevents *events);

private:
    void initialize_io();
    aeffect* load_vst_2x_plugin(const char* path);
    void show_vst_2x_editor(aeffect* effect);
    dispatcher_funcptr m_dispatcher;
    aeffect* m_plugin;
};
}
}
}


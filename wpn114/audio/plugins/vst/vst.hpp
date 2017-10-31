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
#include <wpn114/audio/backend/unit_base.hpp>

using aeffect = AEffect;
using vstint32_t = VstInt32;
using vstint64_t = VstInt64;
using vstintptr_t = VstIntPtr;
using vstevents = VstEvents;

extern "C" {

vstintptr_t VSTCALLBACK
host_callback(aeffect* effect, vstint32_t opcode, vstint32_t index,
              vstintptr_t value, void *ptr, float opt);

}

typedef aeffect* (*vst_plugin_funcptr)(audioMasterCallback host);

typedef vstintptr_t
(*dispatcher_funcptr)(aeffect* effect, vstint32_t opcode, vstint32_t index,
                      vstint32_t value, void *ptr, float opt);

typedef float (*get_parameter_funcptr)(aeffect* effect, vstint32_t index);
typedef void (*set_parameter_funcptr)(aeffect* effect, vstint32_t index, float value);
typedef vstint32_t (*process_events_funcptr) (vstevents *events);
typedef void (*process_funcptr) (aeffect* effect, float **inputs, float **outputs, vstint32_t sample_frames);


namespace wpn114 {
namespace audio {
namespace plugins {

class vst_hdl final : public wpn114::audio::unit_base
{
public:
    vst_hdl(std::string name_with_extension);
    vst_hdl() = delete;
    vst_hdl(const vst_hdl&) = delete;
    vst_hdl(vst_hdl&&) = delete;

    ~vst_hdl();

#ifdef WPN_OSSIA
    void net_expose(ossia::net::device_base* application_node)   override;
#endif

    void show_editor();
    void process_audio(uint16_t num_frames)     override;
    void process_midi(vstevents *events);
    void start();
    void suspend();
    void resume();

private:
    void                _silence_channel(float **channel_data, int num_channels, long num_frames) {}
    aeffect*            _load_vst_2x_plugin(const char* path);
    void                _show_vst_2x_editor(aeffect* effect, const char *plugin_name,
                                            uint16_t width, uint16_t height);
    dispatcher_funcptr  m_dispatcher;
    aeffect*            m_plugin;
    std::string         m_plugin_name;
};
}
}
}


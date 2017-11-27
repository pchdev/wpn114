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
//-------------------------------------------------------------------------------------------------------
using aeffect = AEffect;
using vstint32_t = VstInt32;
using vstint64_t = VstInt64;
using vstintptr_t = VstIntPtr;
using vstevents = VstEvents;
//-------------------------------------------------------------------------------------------------------
extern "C" {

vstintptr_t VSTCALLBACK
host_callback(aeffect* effect, vstint32_t opcode, vstint32_t index,
              vstintptr_t value, void *ptr, float opt);
}

typedef aeffect* (*vst_plugin_funcptr)(audioMasterCallback host);

typedef vstintptr_t
(*dispatcher_funcptr)(aeffect* effect, vstint32_t opcode, vstint32_t index,
                      vstint32_t value, void *ptr, float opt);

typedef float       (*get_parameter_funcptr)(aeffect* effect, vstint32_t index);
typedef void        (*set_parameter_funcptr)(aeffect* effect, vstint32_t index, float value);
typedef vstint32_t  (*process_events_funcptr) (vstevents *events);
typedef void        (*process_funcptr) (aeffect* effect, float **inputs, float **outputs, vstint32_t sample_frames);
//-------------------------------------------------------------------------------------------------------
enum class MIDI
{
    SYSEX                   = 0xf0,
    EOX                     = 0xf7,
    NOTE_OFF                = 0x80,
    NOTE_ON                 = 0x90,
    AFTERTOUCH              = 0xa0,
    CONTINUOUS_CONTROL      = 0xb0,
    PATCH_CHANGE            = 0xc0,
    CHANNEL_PRESSURE        = 0xd0,
    PITCH_BEND              = 0xe0
};
//-------------------------------------------------------------------------------------------------------
typedef std::pair<MIDI,uint8_t> midi_info;
static  std::map<std::string,   midi_info> midi_map;
//-------------------------------------------------------------------------------------------------------
namespace wpn114 {
namespace audio {
namespace plugins {
//-------------------------------------------------------------------------------------------------------
class vst_hdl final : public wpn114::audio::unit_base
        // a vst 2x (3x tbi) handler for units
//-------------------------------------------------------------------------------------------------------
{
public:
    vst_hdl(const char* name_with_extension);
    vst_hdl() = delete;
    vst_hdl(const vst_hdl&) = delete;
    vst_hdl(vst_hdl&&) = delete;
    ~vst_hdl();

#ifdef WPN_CONTROL_OSSIA //---------------------------------------------------------------------------------------
    void net_expose_plugin_tree(ossia::net::node_base& root) override;
#endif //-------------------------------------------------------------------------------------------------

    void show_editor();
    void close_editor();
    void process(uint16_t nsamples) override;
    void process(float** input, uint16_t nsamples) override;
    void preprocess(size_t sample_rate, uint16_t nsamples) override;
    void process_midi(vstevents *events);
    void suspend();
    void resume();
private:
    void        _silence_channel(float **channel_data, uint8_t nchannels, uint16_t nsamples);
    aeffect*    _load_vst_2x_plugin(const char* path);
    void        _load_vst_3x_plugin(const char* path);
#ifdef __APPLE__
    void        _show_vst_2x_editor(aeffect* effect, const char *name, uint16_t width, uint16_t height);
    void        _close_vst_2x_editor();
#endif
    void        _show_vst_3x_editor();

    dispatcher_funcptr      m_dispatcher;
    aeffect*                m_plugin;
    std::string             m_plugin_path;
    //NSWindow*               m_editwindow;
    std::thread             m_editthread;
};
}
}
}


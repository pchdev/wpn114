/*
 *
 * =====================================================================================
 *
 *       Filename:  vst.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  15.10.2017 17:39:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

//-------------------------------------------------------------------------------------------------------
#include <stdlib.h>
#include <iostream>
#include <wpn114/audio/backend/context.hpp>
#include <wpn114/audio/plugins/vst/vst.hpp>
//-------------------------------------------------------------------------------------------------------
#define PARAMNAME_MAXLE 64
#define PROGRAMNAME_MAXLE 64
//-------------------------------------------------------------------------------------------------------
using namespace wpn114::audio::plugins;
//-------------------------------------------------------------------------------------------------------

extern "C"
{
vstintptr_t VSTCALLBACK
host_callback(aeffect* effect, vstint32_t opcode, vstint32_t index, vstintptr_t value, void *ptr,
              float opt)
{
    vstintptr_t result = 0;

    switch(opcode)
    {
    case audioMasterAutomate: break;
    case audioMasterVersion:
        result = 2400;
        break;
    case audioMasterCurrentId:
        break;
    case audioMasterIdle:
        result = 1;
        break;
    case audioMasterGetTime:
        break;
    case audioMasterProcessEvents:
        break;
    case audioMasterIOChanged:
        break;
    case audioMasterGetSampleRate:
        break;
    case audioMasterGetBlockSize:
        break;
    case audioMasterGetInputLatency:
        break;
    case audioMasterGetOutputLatency:
        break;
    case audioMasterGetCurrentProcessLevel:
        break;
    case audioMasterGetAutomationState:
        break;
    case audioMasterGetVendorString:
        break;
    case audioMasterGetProductString:
        break;
    case audioMasterGetVendorVersion:
        break;
    case audioMasterCanDo:
        break;
    case audioMasterGetLanguage:
        break;
    case audioMasterUpdateDisplay:
        break;
    default:
        break;
    }

    return result;
}
}

vst_hdl::vst_hdl(const char* name_with_extension)
{
    std::string name = "/Library/Audio/Plug-Ins/VST/";
    name += name_with_extension;

    m_plugin_path = name;
    m_plugin = this->_load_vst_2x_plugin(name.c_str());

    if  ( m_plugin->magic != kEffectMagic )
    {
        std::cerr << "Plugin's magic number is incorrrect\n";
        return;
    }

    m_dispatcher = (dispatcher_funcptr)(m_plugin->dispatcher);

    m_plugin->getParameter      = (get_parameter_funcptr) m_plugin->getParameter;
    m_plugin->processReplacing  = (process_funcptr) m_plugin->processReplacing;
    m_plugin->setParameter      = (set_parameter_funcptr) m_plugin->setParameter;

    if  (   !m_plugin->numInputs    )
            SET_UTYPE(unit_type::GENERATOR_UNIT);
    else    SET_UTYPE(unit_type::EFFECT_UNIT);

    activate();

    SETN_IN     (m_plugin->numInputs);
    SETN_OUT    (m_plugin->numOutputs);

    if( midi_map.empty())
    {
        midi_map["NOTE_ON"]              = midi_info(MIDI::NOTE_ON, 3);
        midi_map["NOTE_OFF"]             = midi_info(MIDI::NOTE_OFF, 3);
        midi_map["AFTERTOUCH"]           = midi_info(MIDI::AFTERTOUCH, 2);
        midi_map["CONTINUOUS_CONTROL"]   = midi_info(MIDI::CONTINUOUS_CONTROL, 3);
        midi_map["PATCH_CHANGE"]         = midi_info(MIDI::PATCH_CHANGE, 3);
        midi_map["CHANNEL_PRESSURE"]     = midi_info(MIDI::CHANNEL_PRESSURE, 2);
        midi_map["PITCH_BEND"]           = midi_info(MIDI::PITCH_BEND, 2);
    }
}

vst_hdl::~vst_hdl() {}

template <typename T> vstevents* make_vstevent_array(const T& values)
{
    auto res                    = new vstevents();
    res->numEvents              = 1;
    res->events[0]              = new VstEvent();
    res->events[0]->byteSize    = 24;
    res->events[0]->type        = kVstMidiType;
    res->events[0]->flags       = 1;

    VstMidiEvent *event         = (VstMidiEvent*)res->events[0];

    for                 (int i = 0; i < values.size(); ++i)
    event->midiData[i]  = values[i];

    if                  (values.size() < 4)
    event->midiData[3]  = 0;

    event->flags        = kVstMidiEventIsRealtime;
    event->byteSize     = sizeof(VstMidiEvent);

    return res;
}

#ifdef WPN_CONTROL_OSSIA //--------------------------------------------------------------------------------------
void vst_hdl::net_expose_plugin_tree(ossia::net::node_base& root)
{    
    auto midi_node          = root.create_child("MIDI");
    auto params_node        = root.create_child("parameters");
    auto editor_node        = root.create_child("editor");
    auto programs_node      = root.create_child("programs");
    auto open_node          = editor_node->create_child("open");
    auto close_node         = editor_node->create_child("close");
    auto open_param         = open_node->create_parameter(ossia::val_type::IMPULSE);
    auto close_param        = close_node->create_parameter(ossia::val_type::IMPULSE);

    open_param->add_callback([=](const ossia::value& v) {
        this->show_editor();
    });

    close_param->add_callback([=](const ossia::value& v) {
        this->close_editor();
    });

    // Creating MIDI parameters -------------------------------------------------
    for(auto it = midi_map.begin(); it != midi_map.end(); ++it)
    {
        auto node = midi_node->create_child(it->first);

        uint8_t nargs = it->second.second;
        ossia::net::parameter_base* param;

        if(nargs == 2)
        param = node->create_parameter(ossia::val_type::VEC2F);
        else param = node->create_parameter(ossia::val_type::VEC3F);

        param->add_callback([=](const ossia::value& v) {
            //! todo: find a more elegant way to do this...
            if(nargs == 2)
            {
                auto v_array = v.get<ossia::vec2f>();
                v_array[0] += (uint8_t) it->second.first;
                process_midi(make_vstevent_array<ossia::vec2f>(v_array));
            }
            if(nargs == 3)
            {
                auto v_array = v.get<ossia::vec3f>();
                v_array[0] += (uint8_t) it->second.first;
                process_midi(make_vstevent_array<ossia::vec3f>(v_array));
            }
        });
    }

    // Creating float parameters -------------------------------------------
    for(int i = 0; i < m_plugin->numParams; ++i)
    {
        char param_name[PARAMNAME_MAXLE];
        m_dispatcher(m_plugin, effGetParamName, i, 0, &param_name, 0);

        auto node   = params_node->create_child(param_name);
        auto param  = node->create_parameter(ossia::val_type::FLOAT);

        param->add_callback([=](const ossia::value& v) {
            m_plugin->setParameter(m_plugin, i, v.get<float>());
        });

        auto node_domain = ossia::make_domain(0.f, 1.f);
        ossia::net::set_domain(*node, node_domain);
    }

    // programs -----------------------------------------------------------
    for(int i = 0; i < m_plugin->numPrograms; ++i)
    {
        char program_name[PROGRAMNAME_MAXLE];
        m_dispatcher(m_plugin, effGetProgramName, i, 0, &program_name, 0);
        std::cout << program_name << std::endl;
        //! todo
    }
}
#endif //------------------------------------------------------------------------------------------------

void vst_hdl::show_editor()
{
    ERect   *editor_rect    = 0;
    uint16_t width          = 0;
    uint16_t height         = 0;

    m_dispatcher(m_plugin, effEditGetRect, 0, 0, &editor_rect, 0);

    if(editor_rect)
    {
        width   = editor_rect->right - editor_rect->left;
        height  = editor_rect->bottom - editor_rect->top;
    }
    else
    {
        std::cerr << "error, could not get plugin editor's window" << std::endl;
        //! TODO: add error management
    }

#ifdef __APPLE__
    //m_editthread = std::thread(_show_vst_2x_editor, m_plugin, m_plugin_path.c_str(), width, height);
#endif

}

void vst_hdl::close_editor()
{
    m_dispatcher(m_plugin, effEditClose, 0, 0, nullptr, 0);
}

void vst_hdl::preprocess(size_t sample_rate, uint16_t nsamples)
{
    m_dispatcher(m_plugin, effOpen, 0, 0, NULL, 0.0f);
    m_dispatcher(m_plugin, effSetSampleRate, 0, 0, NULL, sample_rate);
    m_dispatcher(m_plugin, effSetBlockSize, 0, nsamples, NULL, 0.0f);
    resume();
}

void vst_hdl::suspend()
{
    m_dispatcher(m_plugin, effMainsChanged, 0, 0, NULL, 0.f);
}

void vst_hdl::resume()
{
    m_dispatcher(m_plugin, effMainsChanged, 0, 1, NULL, 0.f);
}

inline void vst_hdl::_silence_channel(float** channel_data, uint8_t nchannels, uint16_t nsamples)
{
    for(uint8_t ch  = 0; ch < nchannels; ++ch)
        for(uint16_t fr = 0; fr < nsamples; ++fr)
            channel_data[ch][fr] = 0.0f;
}

void vst_hdl::process_midi(vstevents *events)
{
    m_dispatcher(m_plugin, effProcessEvents, 0, 0, events, 0.f);
}

void vst_hdl::process(uint16_t nsamples)
{
    _silence_channel(m_out, m_nout, nsamples);
    m_plugin->processReplacing(m_plugin, nullptr, m_out, nsamples);
}

void vst_hdl::process(float** input, uint16_t nsamples)
{
    _silence_channel(m_out, m_nout, nsamples);
    m_plugin->processReplacing(m_plugin, input, m_out, nsamples);
}


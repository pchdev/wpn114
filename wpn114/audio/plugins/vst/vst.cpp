/*
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
#include <stdlib.h>
#include <iostream>

#include <wpn114/audio/backend/context.hpp>
#include <wpn114/audio/plugins/vst/vst.hpp>

using namespace wpn114::audio::plugins;
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

    if  (m_plugin->magic != kEffectMagic)
        std::cerr << "Plugin's magic number is incorrrect\n";

    m_dispatcher = (dispatcher_funcptr)(m_plugin->dispatcher);

    m_plugin->getParameter      = (get_parameter_funcptr) m_plugin->getParameter;
    m_plugin->processReplacing  = (process_funcptr) m_plugin->processReplacing;
    m_plugin->setParameter      = (set_parameter_funcptr) m_plugin->setParameter;

    SET_UTYPE(      wpn114::audio::unit_type::HYBRID_UNIT);
    SETN_INPUTS(    m_plugin->numInputs);
    SETN_OUTPUTS(   m_plugin->numOutputs);
    SET_ACTIVE;
}

vst_hdl::~vst_hdl() {}

#ifdef WPN_OSSIA //--------------------------------------------------------------------------------------
void vst_hdl::net_expose_plugin_tree(ossia::net::node_base& root)
{
    auto midi_node          = root.create_child("MIDI");
    auto noteon_node        = midi_node->create_child("note_on");
    auto noteoff_node       = midi_node->create_child("note_off");
    auto cc_node            = midi_node->create_child("cc");
    auto pc_node            = midi_node->create_child("program_change");
    auto aftertouch_node    = midi_node->create_child("aftertouch");
    auto polypressure_node  = midi_node->create_child("poly_pressure");
    auto pitchbend_node     = midi_node->create_child("pitchbend");

    auto noteon_param           = noteon_node->create_parameter(ossia::val_type::VEC3F);
    auto noteoff_param          = noteoff_node->create_parameter(ossia::val_type::VEC3F);
    auto cc_param               = cc_node->create_parameter(ossia::val_type::VEC3F);
    auto pc_param               = pc_node->create_parameter(ossia::val_type::VEC3F);
    auto aftertouch_param       = aftertouch_node->create_parameter(ossia::val_type::VEC2F);
    auto polypressure_param     = polypressure_node->create_parameter(ossia::val_type::VEC3F);
    auto pitchbend_param        = pitchbend_node->create_parameter(ossia::val_type::VEC2F);

    noteon_param->add_callback([=](const ossia::value& v) {

        auto v_array = v.get<std::array<float,3>>();

        vstevents res;
        res.numEvents = 1;
        res.events[0] = new VstEvent();
        res.events[0]->byteSize = 24;
        res.events[0]->type = kVstMidiType;
        res.events[0]->flags = 1;

        VstMidiEvent *event = (VstMidiEvent*)res.events[0];
        event->type = kVstMidiType;
        event->midiData[0] = (char) MIDI::NOTE_ON + (char) v_array[0];
        event->midiData[1] = (char) v_array[1]; // index
        event->midiData[2] = (char) v_array[2]; // value
        event->midiData[3] = 0;
        event->flags = kVstMidiEventIsRealtime;
        event->byteSize = sizeof(VstMidiEvent);
        event->reserved1 = 0;
        event->reserved2 = 0;

        res.numEvents = 1;
        process_midi(&res);
    });

    for(int i = 0; i < m_plugin->numParams; ++i)
    {
        char param_name[256];
        m_dispatcher(m_plugin, effGetParamName, i, 0, &param_name, 0);
        auto node = root.create_child(param_name);
        auto param = node->create_parameter(ossia::val_type::FLOAT);

        param->add_callback([=](const ossia::value& v) {
            m_plugin->setParameter(m_plugin, i, v.get<float>());
        });
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
    }

    this->_show_vst_2x_editor(this->m_plugin, m_plugin_path.c_str(), width, height);
}

void vst_hdl::preprocessing(size_t sample_rate, uint16_t nsamples)
{
    m_dispatcher(m_plugin, effOpen, 0, 0, NULL, 0.0f);
    m_dispatcher(m_plugin, effSetSampleRate, 0, 0, NULL, sample_rate);
    m_dispatcher(m_plugin, effSetBlockSize, 0, nsamples, NULL, 0.0f);

    this->resume();
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

inline void vst_hdl::process_midi(vstevents *events)
{
    m_dispatcher(m_plugin, effProcessEvents, 0, 0, events, 0.f);
}

void vst_hdl::process_audio(uint16_t nsamples)
{
    //_silence_channel(m_input_buffer, m_num_inputs, samples_per_buffer);
    _silence_channel(m_output_buffer, m_num_outputs, nsamples);
    m_plugin->processReplacing(m_plugin, nullptr, m_output_buffer, nsamples);
}

void vst_hdl::process_audio(float** input, uint16_t nsamples)
{
    _silence_channel(m_output_buffer, m_num_outputs, nsamples);
    m_plugin->processReplacing(m_plugin, input, m_output_buffer, nsamples);
}


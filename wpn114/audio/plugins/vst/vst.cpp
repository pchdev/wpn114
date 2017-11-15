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

extern "C" {

vstintptr_t VSTCALLBACK
host_callback(aeffect* effect, vstint32_t opcode, vstint32_t index,
              vstintptr_t value, void *ptr, float opt)
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

wpn114::audio::plugins::vst_hdl::vst_hdl(const char* name_with_extension)
{
    std::string name = "/Library/Audio/Plug-Ins/VST/";
    name += name_with_extension;
    m_plugin_name = name;

    m_plugin = this->_load_vst_2x_plugin(m_plugin_name.c_str());

    if  (m_plugin->magic != kEffectMagic)
        std::cerr << "Plugin's magic number is incorrrect\n";

    m_dispatcher = (dispatcher_funcptr)(m_plugin->dispatcher);

    m_plugin->getParameter      = (get_parameter_funcptr) m_plugin->getParameter;
    m_plugin->processReplacing  = (process_funcptr) m_plugin->processReplacing;
    m_plugin->setParameter      = (set_parameter_funcptr) m_plugin->setParameter;

    SET_UTYPE(      wpn114::audio::unit_type::HYBRID_UNIT);
    SETN_INPUTS(    m_plugin->numInputs);
    SETN_OUTPUTS(   m_plugin->numOutputs);

    SET_ACTIVE
}

wpn114::audio::plugins::vst_hdl::~vst_hdl() {}

#ifdef WPN_OSSIA
void wpn114::audio::plugins::vst_hdl::net_expose(ossia::net::device_base* application_node)
{
    for(int i = 0; i < m_plugin->numParams; ++i)
    {
        char name[256];
        m_dispatcher(m_plugin, effGetParamName, 0, 0, &name, 0);
        std::cout << name << std::endl;
    }
}
#endif

void wpn114::audio::plugins::vst_hdl::show_editor()
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

    this->_show_vst_2x_editor(this->m_plugin, m_plugin_name.c_str(), width, height);
}

void wpn114::audio::plugins::vst_hdl::initialize(uint16_t samples_per_buffer)
{
    m_dispatcher(m_plugin, effOpen, 0, 0, NULL, 0.0f);
    m_dispatcher(m_plugin, effSetSampleRate, 0, 0, NULL, 44100);
    m_dispatcher(m_plugin, effSetBlockSize, 0, samples_per_buffer, NULL, 0.0f);

    this->resume();
}

void wpn114::audio::plugins::vst_hdl::suspend()
{
    m_dispatcher(m_plugin, effMainsChanged, 0, 0, NULL, 0.f);
}

void wpn114::audio::plugins::vst_hdl::resume()
{
    m_dispatcher(m_plugin, effMainsChanged, 0, 1, NULL, 0.f);
}

inline void wpn114::audio::plugins::vst_hdl::_silence_channel(float** channel_data, uint8_t num_channels, uint16_t samples_per_buffer)
{
    for(uint8_t ch  = 0; ch < num_channels; ++ch)
        for(uint16_t fr = 0; fr < samples_per_buffer; ++fr)
            channel_data[ch][fr] = 0.0f;
}

inline void wpn114::audio::plugins::vst_hdl::process_midi(vstevents *events)
{
    m_dispatcher(m_plugin, effProcessEvents, 0, 0, events, 0.f);
}

void wpn114::audio::plugins::vst_hdl::process_audio(uint16_t samples_per_buffer)
{
    _silence_channel(m_input_buffer, m_num_inputs, samples_per_buffer);
    _silence_channel(m_output_buffer, m_num_outputs, samples_per_buffer);

    m_plugin->processReplacing(m_plugin, m_input_buffer, m_output_buffer, samples_per_buffer);
}



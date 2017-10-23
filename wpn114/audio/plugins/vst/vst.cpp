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

wpn114::audio::plugins::vst_hdl::vst_hdl(std::string name_with_extension) :
    m_plugin_name(name_with_extension)
{
    m_plugin = this->_load_vst_2x_plugin(m_plugin_name.c_str());

    if(m_plugin->magic != kEffectMagic)
    {
        std::cerr << "Plugin's magic number is incorrrect\n";
    }

    m_dispatcher = (dispatcher_funcptr)(m_plugin->dispatcher);

    m_plugin->getParameter = (get_parameter_funcptr) m_plugin->getParameter;
    m_plugin->processReplacing = (process_funcptr) m_plugin->processReplacing;
    m_plugin->setParameter = (set_parameter_funcptr) m_plugin->setParameter;
}

wpn114::audio::plugins::vst_hdl::~plugin_handler() {}

#ifdef WPN_OSSIA
void wpn114::audio::plugins::vst_hdl::net_expose(std::shared_ptr<ossia::net::node_base> application_node)
{}
#endif

void wpn114::audio::plugins::vst_hdl::show_editor()
{
    ERect   *editor_rect    = 0;
    uint16_t width          = 0;
    uint16_t height         = 0;

    m_dispatcher(m_plugin, effEditGetRect, 0, 0, &editor_rect, 0);

    if(editor_rect)
    {
        width = editor_rect->right - editor_rect->left;
        height = editor_rect->bottom - editor_rect->top;
    }
    else
    {
        std::cerr << "error, could not get plugin editor's window" << std::endl;
    }

    this->_show_vst_2x_editor(this->m_plugin, m_plugin_name.c_str(), width, height);
}

void wpn114::audio::plugins::vst_hdl::start()
{
    m_dispatcher(m_plugin, effOpen, 0, 0, NULL, 0.0f);
    m_dispatcher(m_plugin, effSetSampleRate, 0, 0, NULL, wpn114::audio::context.sample_rate);
    m_dispatcher(m_plugin, effSetBlockSize, 0, wpn114::audio::context.blocksize, NULL, 0.0f);

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

void wpn114::audio::plugins::vst_hdl::process_audio()
{
    silence_channel(m_output_buffer, m_num_outputs, wpn114::audio::context.blocksize);
    silence_channel(m_input_buffer, m_num_inputs, wpn114::audio::context.blocksize);

    m_plugin->processReplacing(m_plugin, nullptr, m_output_buffer, wpn114::audio::context.blocksize);
}

inline void wpn114::audio::plugins::vst_hdl::process_midi(vstevents *events)
{
    m_dispatcher(m_plugin, effProcessEvents, 0, 0, events, 0.f);
}

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

wpn114::audio::units::plugin_handler::plugin_handler(std::string name) :
    m_plugin_name(name)
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

wpn114::audio::units::plugin_handler::~plugin_handler()
{

}

void wpn114::audio::units::plugin_handler::net_expose()
{

}

void wpn114::audio::units::plugin_handler::show()
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

void wpn114::audio::units::plugin_handler::start()
{
    m_dispatcher(m_plugin, effOpen, 0, 0, NULL, 0.0f);
    m_dispatcher(m_plugin, effSetSampleRate, 0, 0, NULL, wpn114::audio::context.sample_rate);
    m_dispatcher(m_plugin, effSetBlockSize, 0, wpn114::audio::context.blocksize, NULL, 0.0f);

    this->resume();
}

void wpn114::audio::units::plugin_handler::suspend()
{
    m_dispatcher(m_plugin, effMainsChanged, 0, 0, NULL, 0.f);
}

void wpn114::audio::units::plugin_handler::resume()
{
    m_dispatcher(m_plugin, effMainsChanged, 0, 1, NULL, 0.f);
}

void wpn114::audio::units::plugin_handler::initialize_io()
{
    uint32_t blocksize = wpn114::audio::context.blocksize;

    // initialize IO buffers
    if(m_input_buffer)
    {
        m_input_buffer = new float*[m_num_inputs];
        for(int i = 0; i < m_num_inputs; ++i)
        {
            m_input_buffer[i] = new float[blocksize];
        }
    }

    if(m_output_buffer)
    {
        m_output_buffer = new float*[m_num_outputs];
        for(int i = 0; i < m_num_outputs; ++i)
        {
            m_output_buffer[i] = new float[blocksize];
        }
    }
}

inline void wpn114::audio::units::plugin_handler::process_audio()
{
    silence_channel(m_output_buffer, m_num_outputs, wpn114::audio::context.blocksize);
    silence_channel(m_input_buffer, m_num_inputs, wpn114::audio::context.blocksize);

    m_plugin->processReplacing(m_plugin, nullptr, m_output_buffer, wpn114::audio::context.blocksize);
}

inline float wpn114::audio::units::plugin_handler::get_framedata(uint16_t channel, uint32_t frame) const
{
    return m_output_buffer[channel][frame];
}

inline void wpn114::audio::units::plugin_handler::silence_channel(float **channel_data, int num_channels, long num_frames)
{
    for(int channel =0; channel < num_channels; ++channel)
        for(long frame = 0; frame < num_frames; ++frame)
            channel_data[channel][frame] = 0.f;
}

inline void wpn114::audio::units::plugin_handler::process_midi(vstevents *events)
{
    m_dispatcher(m_plugin, effProcessEvents, 0, 0, events, 0.f);
}

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

#include "vst.h"

wpn114::audio::vst::plugin_handler::plugin_handler(const char *name)
{
    m_plugin = this->load_vst_2x_plugin(name);
    if(m_plugin->magic != kEffectMagic)
    {
        std::cerr << "Plugin's magic number is incorrrect\n";
        return -1;
    }

    m_dispatcher = (dispatcher_funcptr)(m_plugin->dispatcher);

    m_plugin->getParameter = (get_parameter_funcptr) m_plugin->getParameter;
    m_plugin->processReplacing = (process_funcptr) m_plugin->processReplacing;
    m_plugin->setParameter = (set_parameter_funcptr) m_plugin->setParameter;
}

wpn114::audio::vst::plugin_handler::~plugin_handler()
{

}

void wpn114::audio::vst::plugin_handler::start_plugin(float sample_rate, int blocksize)
{
    m_dispatcher(m_plugin, effOpen, 0, 0, NULL, 0.0f);
    m_dispatcher(m_plugin, effSetSampleRate, 0, 0, NULL, sample_rate);
    m_dispatcher(m_plugin, effSetBlockSize, 0, blocksize, NULL, 0.0f);

    this->resume_plugin();
}

void wpn114::audio::vst::plugin_handler::suspend_plugin()
{
    m_dispatcher(m_plugin, effMainsChanged, 0, 0, NULL, 0.f);
}

void wpn114::audio::vst::plugin_handler::resume_plugin()
{
    m_dispatcher(m_plugin, effMainsChanged, 0, 1, NULL, 0.f);
}

void wpn114::audio::vst::plugin_handler::initialize_io()
{
    //! TODO
}

void wpn114::audio::vst::plugin_handler::process_audio(float **inputs, float **outputs, long num_frames)
{
    this->silence_channel(outputs, num_channels, num_frames);
    this->silence_channel(inputs, num_channels, num_frames);
    m_plugin->processReplacing(m_plugin, inputs, outputs, num_frames);
}

void wpn114::audio::vst::plugin_handler::silence_channel(float **channel_data, int num_channels, long num_frames)
{
    for(int channel =0; channel < num_channels; ++channel)
        for(long frame = 0; frame < num_frames; ++frame)
            channel_data[channel][frame] = 0.f;
}

void wpn114::audio::vst::plugin_handler::process_midi(vstevents *events)
{
    m_dispatcher(m_plugin, effProcessEvents, 0, 0, events, 0.f);
}

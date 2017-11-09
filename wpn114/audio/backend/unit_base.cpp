/*
 * =====================================================================================
 *
 *       Filename:  unit_base.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  26.10.2017 14:06:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <wpn114/audio/backend/unit_base.hpp>

using namespace wpn114::audio;

unit_base::~unit_base()
{
    for (int i = 0; i < m_num_outputs; ++i)
        delete m_output_buffer[i];

    delete m_output_buffer;

    if(m_unit_type == unit_type::EFFECT_UNIT ||
            m_unit_type == unit_type::HYBRID_UNIT)
    {
        for (int i = 0; i < m_num_inputs; ++i)
            delete m_input_buffer[i];

        delete m_input_buffer;
    }
}

void unit_base::initialize_io(uint16_t samples_per_buffer)
{
    // allocate input/output buffers if necessary
    m_output_buffer = new float*[m_num_outputs];

    for (int i = 0; i < m_num_outputs; ++i)
        m_output_buffer[i] = new float[samples_per_buffer];

    if  (m_unit_type == unit_type::EFFECT_UNIT ||
         m_unit_type == unit_type::HYBRID_UNIT)
    {
        m_input_buffer = new float*[m_num_inputs];

        for (int i = 0; i < m_num_inputs; ++i)
            m_input_buffer[i] = new float[samples_per_buffer];
    }
}

unit_type unit_base::get_unit_type()
{
    return m_unit_type;
}

float unit_base::get_framedata(uint8_t channel, uint16_t frame) const
{
    return m_output_buffer[channel][frame];
}

uint8_t unit_base::get_num_channels() const
{
    return m_num_outputs;
}

bool unit_base::is_active()
{
    return m_active;
}

void unit_base::activate()
{
    m_active = true;
}

void unit_base::deactivate()
{
    m_active = false;
}

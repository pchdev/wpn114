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
}

void unit_base::bufalloc(uint16_t samples_per_buffer)
{
    m_output_buffer = new float*[m_num_outputs];

    for (int i = 0; i < m_num_outputs; ++i)
    {
        m_output_buffer[i] = new float[samples_per_buffer];
        memset(m_output_buffer[i], 0.f, sizeof(samples_per_buffer));
    }
}

unit_type unit_base::get_unit_type() const
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

bool unit_base::is_active() const
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

float** unit_base::get_output_buffer()
{
    return m_output_buffer;
}

#ifdef WPN_AUDIO_AUX

void unit_base::add_aux_send(aux_unit &aux)
{
    aux.add_sender(this, 1.f);
}

#endif

//-------------------------------------------------------------------------------------------------------
#ifdef WPN_AUDIO_AUX
//-------------------------------------------------------------------------------------------------------
aux_unit::aux_unit()
{
    SET_UTYPE(unit_type::EFFECT_UNIT);
    SET_ACTIVE;
}

aux_unit::aux_unit(std::unique_ptr<unit_base> receiver) : m_receiver(std::move(receiver))
{
    SET_UTYPE( unit_type::EFFECT_UNIT );
    SET_ACTIVE;
}

aux_unit::~aux_unit() {}

#ifdef WPN_OSSIA //--------------------------------------------------------------------------------------
void aux_unit::net_expose(ossia::net::device_base *application_node, const char* name)
{

}
#endif //------------------------------------------------------------------------------------------------

void aux_unit::preprocessing(size_t sample_rate, uint16_t nsamples)
{
    m_receiver->preprocessing(sample_rate, nsamples);
}

void aux_unit::process_audio(float **input_buffer, uint16_t nsamples) {}
void aux_unit::process_audio(uint16_t nsamples)
{
    for(uint16_t i = 0; i < nsamples; ++i)
    {
        for(uint8_t c = 0; i < N_OUTPUTS; ++c)
        {
            OUT[c][i] = 0.f;

            for(uint8_t u = 0; i < m_sends.size(); ++u)
            {
                // mix input sources
                OUT[c][i] += m_sends[u].m_sender->get_output_buffer()[c][i] * m_sends[u].m_level;
            }
        }
    }

    // pass mixed output buffer to receiver unit
    m_receiver->process_audio(OUT, nsamples);
}

void aux_unit::add_sender(unit_base *sender, float level)
{
    aux_send send = {sender,level};
    m_sends.push_back(send);
}

void aux_unit::set_receiver(std::unique_ptr<unit_base> receiver)
{
    m_receiver = std::move(receiver);
}

#endif

//-------------------------------------------------------------------------------------------------------
#ifdef WPN_AUDIO_TRACKS
//-------------------------------------------------------------------------------------------------------
track_unit::track_unit()
{
    SET_UTYPE(unit_type::HYBRID_UNIT);
    SET_ACTIVE;
}

track_unit::~track_unit() {}

#ifdef WPN_OSSIA //--------------------------------------------------------------------------------------
void track_unit::net_expose(ossia::net::device_base *application_node, const char* name)
{

}
#endif //------------------------------------------------------------------------------------------------

void track_unit::preprocessing(size_t sample_rate, uint16_t nsamples)
{
    for (auto& unit : m_units)
        unit->preprocessing(sample_rate, nsamples);
}

void track_unit::process_audio(float **input_buffer, uint16_t nsamples)
{
    OUT = input_buffer;
    for(auto& unit : m_units)
    {
        unit->process_audio(OUT, nsamples);
    }
}

void track_unit::process_audio(uint16_t nsamples)
{
    for (auto& unit : m_units)
    {
        if(unit->get_unit_type() == unit_type::GENERATOR_UNIT)
        {
            unit->process_audio(nsamples);
        }

        else if(unit->get_unit_type() == unit_type::EFFECT_UNIT ||
                unit->get_unit_type() == unit_type::HYBRID_UNIT )
        {
            unit->process_audio(OUT, nsamples);
        }

        OUT = unit->get_output_buffer();
    }
}

void track_unit::add_unit(unit_base *unit)
{
    m_units.push_back(unit);
}

void track_unit::remove_unit(unit_base *unit)
{
    m_units.erase(  std::remove(m_units.begin(), m_units.end(), unit),
                    m_units.end());
}

#endif

//-------------------------------------------------------------------------------------------------------
#ifdef WPN_AUDIO_SNDFILE
//-------------------------------------------------------------------------------------------------------

buffer_unit::~buffer_unit()
{
    CLEAR_SFBUF;
}

#endif

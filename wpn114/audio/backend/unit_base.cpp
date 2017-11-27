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
    for     (int i = 0; i < m_num_outputs; ++i)
    delete  m_output_buffer[i];

    delete  m_output_buffer;
}

void unit_base::bufalloc(uint16_t nsamples)
{
    m_output_buffer = new float*[m_num_outputs];

    for (int i = 0; i < m_num_outputs; ++i)
    {
        m_output_buffer[i] = new float[nsamples];
        memset(m_output_buffer[i], 0.f, sizeof(nsamples));
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
#ifdef WPN_CONTROL_OSSIA // -------------------------------------------------------------------------------------
    auto parameter = m_netnode->find_child("active")->get_parameter();
    parameter->set_value(true);
#endif //------------------------------------------------------------------------------------------------
}

void unit_base::deactivate()
{
    m_active = false;
#ifdef WPN_CONTROL_OSSIA // -------------------------------------------------------------------------------------
    auto parameter = m_netnode->find_child("active")->get_parameter();
    parameter->set_value(false);
#endif //------------------------------------------------------------------------------------------------
}

float** unit_base::get_output_buffer()
{
    return m_output_buffer;
}

float unit_base::get_level() const
{
    return m_level;
}

void unit_base::set_level(float level)
{
    m_level = level;
#ifdef WPN_CONTROL_OSSIA // -------------------------------------------------------------------------------------
    auto parameter = m_netnode->find_child("level")->get_parameter();
    parameter->set_value(level);
#endif //------------------------------------------------------------------------------------------------
}

#ifdef WPN_CONTROL_OSSIA //--------------------------------------------------------------------------------------
void unit_base::net_expose(ossia::net::node_base &application_node)
{
    m_netnode           = application_node.create_child(m_netname);
    auto master_node    = m_netnode->create_child("master");
    auto level_node     = master_node->create_child("level");
    auto active_node    = master_node->create_child("active");
    auto level_param    = level_node->create_parameter(ossia::val_type::FLOAT);
    auto active_param   = active_node->create_parameter(ossia::val_type::BOOL);

    level_param->add_callback([&](const ossia::value& v) {
        m_level = v.get<float>();
    });

    active_param->add_callback([&](const ossia::value& v) {
        m_active = v.get<bool>();
    });

    active_param->set_value(m_active);
    auto level_domain = ossia::make_domain(0.f, 1.f);
    ossia::net::set_domain(*level_node, level_domain);

    net_expose_plugin_tree(*m_netnode);
}

void unit_base::net_expose(ossia::net::node_base &application_node, std::string name)
{
    m_netname = name;
    net_expose(application_node);
}

void unit_base::set_netname(std::string name)
{
    m_netname = name;
}

const std::string& unit_base::get_netname() const
{
    return m_netname;
}
#endif //------------------------------------------------------------------------------------------------

#ifdef WPN_AUDIO_AUX //----------------------------------------------------------------------------------
void unit_base::add_aux_send(aux_unit &aux)
{
    aux.add_sender(this, 1.f);
}
#endif //------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------
#ifdef WPN_AUDIO_AUX
//-------------------------------------------------------------------------------------------------------
aux_unit::aux_unit()
{
    SET_UTYPE   (unit_type::EFFECT_UNIT);
    SETN_OUT    (0);

    deactivate();
}

aux_unit::aux_unit(std::unique_ptr<unit_base> receiver) : m_receiver(std::move(receiver))
{
    SET_UTYPE   (unit_type::EFFECT_UNIT );
    SETN_OUT    (receiver->get_num_channels());

    activate();
}

aux_unit::~aux_unit() {}

#ifdef WPN_CONTROL_OSSIA //--------------------------------------------------------------------------------------
void aux_unit::net_expose_plugin_tree(ossia::net::node_base& root)
{
    if  (m_receiver.get())
        m_receiver->net_expose(*m_netnode);
}
#endif //------------------------------------------------------------------------------------------------

void aux_unit::preprocessing(size_t sample_rate, uint16_t nsamples)
{
    m_receiver->bufalloc(nsamples);
    m_receiver->preprocessing(sample_rate, nsamples);
}

void aux_unit::process_audio(float **input_buffer, uint16_t nsamples) {}
void aux_unit::process_audio(uint16_t nsamples)
{
    for(uint16_t i = 0; i < nsamples; ++i)
    {
        for(uint8_t c = 0; c < N_OUT; ++c)
        {
            OUT[c][i] = 0.f;

            for(uint8_t u = 0; u < m_sends.size(); ++u)
                // mix input sources
                OUT[c][i] += m_sends[u].m_sender->get_framedata(c, i) * m_sends[u].m_level;
        }
    }

    // pass mixed output buffer to receiver unit
    //! DO NOT REGISTER THE RECEIVER TO THE AUDIO_BACKEND_HDL
    m_receiver->process_audio(OUT, nsamples);
}

float aux_unit::get_framedata(uint8_t channel, uint16_t frame) const
{
    return m_receiver->get_framedata(channel, frame);
}

void aux_unit::add_sender(unit_base *sender, float level)
{
    if ( m_sends.empty() ) activate();
    aux_send send = {sender,level};
    m_sends.push_back(send);

#ifdef WPN_CONTROL_OSSIA //--------------------------------------------------------------------------------------
    if(m_netnode)
    {
        ossia::net::node_base* send_node    = m_netnode->find_child("sends");
        if(!send_node) send_node            = m_netnode->create_child("sends");

        auto sender_node        = send_node->create_child(sender->get_netname());
        auto sends_level_node   = sender_node->create_child("level");
        auto sends_level_param  = sends_level_node->create_parameter(ossia::val_type::FLOAT);

        sends_level_param       ->set_value(level);
        auto node_domain        = ossia::make_domain(0.f, 1.f);
        ossia::net::set_domain(*sends_level_node, node_domain);
    }
#endif //------------------------------------------------------------------------------------------------
}

void aux_unit::set_receiver(std::unique_ptr<unit_base> receiver)
{
    activate();
    m_receiver = std::move(receiver);
    SETN_OUT(m_receiver->get_num_channels());
}

#endif //------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------
#ifdef WPN_AUDIO_TRACKS
//-------------------------------------------------------------------------------------------------------
track_unit::track_unit()
{
    SET_UTYPE   (unit_type::GENERATOR_UNIT);
    SETN_OUT    (0);

    deactivate();
}

track_unit::~track_unit() {}

#ifdef WPN_CONTROL_OSSIA //--------------------------------------------------------------------------------------
void track_unit::net_expose_plugin_tree(ossia::net::node_base& application_node)
{
    for(auto& unit : m_units)
        unit->net_expose(*m_netnode);
}
#endif //------------------------------------------------------------------------------------------------

void track_unit::preprocessing(size_t sample_rate, uint16_t nsamples)
{
    for (auto& unit : m_units)
    {
        unit->bufalloc(nsamples);
        unit->preprocessing(sample_rate, nsamples);
    }
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
    if(m_units.empty()) activate();
    m_units.push_back(unit);
}

void track_unit::remove_unit(unit_base *unit)
{
    m_units.erase(  std::remove(m_units.begin(), m_units.end(), unit),
                    m_units.end());

    if(m_units.empty()) deactivate();
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

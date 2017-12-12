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
    for     (int i = 0; i < m_nout; ++i)
    delete  m_out[i];

    delete  m_out;
}

void unit_base::bufalloc(uint16_t nsamples)
{
    m_out = new float*[m_nout];

    for (int i = 0; i < m_nout; ++i)
    {
        m_out[i] = new float[nsamples];
        memset(m_out[i], 0.f, sizeof(nsamples));
    }
}

unit_type unit_base::uttype() const
{
    return m_uttype;
}

float unit_base::framedata(uint8_t channel, uint16_t frame) const
{
    return m_out[channel][frame];
}

uint8_t unit_base::nchannels() const
{
    return m_nout;
}

bool unit_base::active() const
{
    return m_active;
}

void unit_base::activate()
{
    m_active = true;
#ifdef WPN_CONTROL_OSSIA // -------------------------------------------------------------------------------------
    if(m_netnode)
    {
        auto parameter = m_netnode->find_child("active")->get_parameter();
        parameter->set_value(true);
    }
#endif //------------------------------------------------------------------------------------------------
}

void unit_base::deactivate()
{
    m_active = false;
#ifdef WPN_CONTROL_OSSIA // -------------------------------------------------------------------------------------
    if(m_netnode)
    {
        auto parameter = m_netnode->find_child("active")->get_parameter();
        parameter->set_value(false);
    }
#endif //------------------------------------------------------------------------------------------------
}

float** unit_base::out()
{
    return m_out;
}

float unit_base::level() const
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
void unit_base::net_expose(ossia::net::node_base& application_node)
{
    m_netnode = application_node.create_child(m_netname);

#ifdef WPN_CONTROL_OSSIA_TOML
    wpn114::net::parse_toml(m_netnode, "unit_base.toml");
#else
    declare_parameter<bool>("/master/active", m_active, false);
    auto level = declare_parameter<float>("/master/level", m_level, { 0.f, 1.f }, 1.f);
    level.set_bounding(ossia::bounding_mode::LOW);
#endif
    net_expose_plugin_tree(*m_netnode);
}

void unit_base::net_expose(ossia::net::node_base& application_node, std::string name)
{
    m_netname = name;
    net_expose(application_node);
}

void unit_base::set_netname(std::string name)
{
    m_netname = name;
}

const std::string& unit_base::netname() const
{
    return m_netname;
}
#endif //------------------------------------------------------------------------------------------------

#ifdef WPN_AUDIO_AUX //----------------------------------------------------------------------------------
void unit_base::add_aux_send(aux_unit& aux)
{
    aux.add_sender(*this, 1.f);
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
    SET_UTYPE   (unit_type::EFFECT_UNIT);
    SETN_OUT    (receiver->nchannels());

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

void aux_unit::preprocess(size_t srate, uint16_t nsamples)
{
    if  (m_receiver.get())
    {
        m_receiver->bufalloc(nsamples);
        m_receiver->preprocess(srate, nsamples);
    }
}

void aux_unit::process(float** inputs, uint16_t nsamples) {}
void aux_unit::process(uint16_t nsamples)
{
    for(uint16_t i = 0; i < nsamples; ++i)
    {
        for(uint8_t c = 0; c < N_OUT; ++c)
        {
            OUT[c][i] = 0.f;

            for(uint8_t u = 0; u < m_sends.size(); ++u)
                // mix input sources
                OUT[c][i] += m_sends[u].m_sender->framedata(c,i) * m_sends[u].m_level;
        }
    }

    // pass mixed output buffer to receiver unit
    //! DO NOT REGISTER THE RECEIVER TO THE AUDIO_BACKEND_HDL
    m_receiver->process(OUT, nsamples);
}

float aux_unit::framedata(uint8_t channel, uint16_t frame) const
{
    return m_receiver->framedata(channel, frame);
}

void aux_unit::add_sender(unit_base& sender, float level)
{
    if ( m_sends.empty() ) activate();
    aux_send send = {&sender,level};
    m_sends.push_back(send);

#ifdef WPN_CONTROL_OSSIA //------------------------------------------------------------------------------
    if(m_netnode)
    {
        auto addr   = (std::string) "/sends/" + sender.netname() + "/level";
        auto send_p = declare_parameter<float>(addr, m_sends[m_sends.size()-1], {0.f, 1.f}, level);
        send_p.set_bounding(ossia::bounding_mode::LOW);
    }
#endif //------------------------------------------------------------------------------------------------
}

void aux_unit::set_receiver(std::unique_ptr<unit_base> receiver)
{
    activate();
    m_receiver = std::move(receiver);
    SETN_OUT(m_receiver->nchannels());
}

/*std::ostream& operator<<(std::ostream& unit, const aux_unit& aux)
{
    aux.set_receiver(unit);
}*/

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

void track_unit::preprocess(size_t srate, uint16_t nsamples)
{
    for (auto& unit : m_units)
    {
        unit->bufalloc(nsamples);
        unit->preprocess(srate, nsamples);
    }
}

void track_unit::process(float **inputs, uint16_t nsamples)
{
    OUT = inputs;
    for(auto& unit : m_units)
    {
        unit->process(OUT, nsamples);
    }
}

void track_unit::process(uint16_t nsamples)
{
    for (auto& unit : m_units)
    {
        if(unit->uttype() == unit_type::GENERATOR_UNIT)
        {
            unit->process(nsamples);
        }

        else if(unit->uttype() == unit_type::EFFECT_UNIT ||
                unit->uttype() == unit_type::HYBRID_UNIT )
        {
            unit->process(OUT, nsamples);
        }

        OUT = unit->out();
    }
}

void track_unit::add_unit(unit_base& unit)
{
    if(m_units.empty()) activate();
    m_units.push_back(&unit);
}

void track_unit::remove_unit(unit_base& unit)
{
    m_units.erase(  std::remove(m_units.begin(), m_units.end(), &unit),
                    m_units.end());

    if(m_units.empty()) deactivate();
}

#endif

/*std::ostream& operator<<(std::ostream& unit, const track_unit& track)
{
    track.add_unit(&unit);
}

std::ostream& operator>>(std::ostream& unit, const track_unit& track)
{
    track.remove_unit(&unit);
}*/

//-------------------------------------------------------------------------------------------------------
#ifdef WPN_AUDIO_SNDFILE
//-------------------------------------------------------------------------------------------------------
buffer_unit::~buffer_unit()
{
    if(SFBUF.data) SFBUF_CLEAR;
}
#endif

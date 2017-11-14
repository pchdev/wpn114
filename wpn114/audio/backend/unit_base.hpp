#pragma once
#include <cstdint>
#include <memory>

#ifdef WPN_OSSIA
    #include <ossia/ossia.hpp>
#endif

namespace wpn114 {
namespace audio {

enum class unit_type
{
    ANALYSIS_UNIT   = 0,
    GENERATOR_UNIT  = 1,
    EFFECT_UNIT     = 2,
    HYBRID_UNIT     = 3
};

class unit_base
{

public:

#ifdef WPN_AUDIO_SENDS
struct aux_send
{
    unit_base*  m_target;
    float       m_level;
};
#endif

    virtual ~unit_base();

#ifdef WPN_OSSIA
    virtual void net_expose(ossia::net::device_base* application_node) = 0;
    // gets called whenever we want to expose the parameters' unit to the network
#endif

    virtual void process_audio(uint16_t samples_per_buffer) = 0;
    virtual void process_audio(float** input_buffer, uint16_t samples_per_buffer) = 0;
    // the unit's audio callback
    virtual void initialize(uint16_t samples_per_buffer) = 0;
    // preparing the units user-defined audio processing

    void        initialize_io(uint16_t samples_per_buffer);
    float       get_framedata(uint8_t channel, uint16_t frame) const;

    uint8_t     get_num_channels() const;
    unit_type   get_unit_type();    

    void        activate();
    void        deactivate();
    bool        is_active();

protected:
#define IN                  m_input_buffer
#define OUT                 m_output_buffer
#define SET_UTYPE(u)        m_unit_type = u;
#define SETN_INPUTS(n)      m_num_inputs = n;
#define SETN_OUTPUTS(n)     m_num_outputs = n;
#define N_OUTPUTS           m_num_outputs
#define N_INPUTS            m_num_inputs
#define SET_ACTIVE          m_active = true;
#define SET_INACTIVE        m_active = false;
#define SET_NAME            m_name = name;
#define SET_LEVEL           m_level = level;

    std::string     m_name;
    bool            m_active;
    float           m_level;
    uint8_t         m_num_inputs;
    uint8_t         m_num_outputs;
    float**         m_output_buffer;
    float**         m_vu;
    unit_type       m_unit_type;

#ifdef WPN_AUDIO_SENDS
    std::vector<aux_send>   m_aux_sends;
#endif

#ifdef WPN_OSSIA
    ossia::net::node_base*  m_ossia_rootnode;
#endif

};
}
}

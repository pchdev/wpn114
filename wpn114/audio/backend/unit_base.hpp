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
    ~unit_base() {}

#ifdef WPN_OSSIA
    virtual void net_expose(std::shared_ptr<ossia::net::node_base> application_node) {}
    // gets called whenever we want to expose the parameters' unit to the network
#endif

    virtual void process_audio(uint32_t num_frames) = 0;
    // the unit's audio callback
    virtual void initialize() = 0;
    // preparing the units user-defined audio processing

    void        initialize_io() {}
                // filling the out buffers
    unit_type   get_unit_type() { return m_unit_type; }
    float       get_framedata(uint16_t channel, uint32_t frame) const {}


protected:
#define IN(c,f)             m_input_buffer[c][f]
#define OUT(c,f,v)          m_output_buffer[c][f] = v;
#define SET_UTYPE(u)        m_unit_type = u;
#define SETN_INPUTS(n)      m_num_inputs = n;
#define SETN_OUTPUTS(n)     m_num_outputs = n;

    int         m_num_inputs;
    int         m_num_outputs;
    float**     m_input_buffer;
    float**     m_output_buffer;
    unit_type   m_unit_type;

#ifdef WPN_OSSIA
    std::shared_ptr<ossia::net::node_base>  m_ossia_rootnode;
#endif

};
}
}

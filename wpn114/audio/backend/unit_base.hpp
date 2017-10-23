#pragma once

#include <cstdint>

namespace wpn114 {
namespace audio {

enum unit_type
{
    ANALYSIS_UNIT   = 0,
    GENERATOR_UNIT  = 1,
    EFFECT_UNIT     = 2,
    HYBRID_UNIT     = 3
};

class unit_base
{
public:
    virtual ~unit_base() {}
    virtual void net_expose()       = 0;
    virtual void initialize()       = 0;
    virtual void process_audio()    = 0;

    virtual float get_framedata     (uint16_t channel, uint32_t frame) const = 0;
    virtual unit_type get_unit_type () const = 0;
    //virtual void process_midi() = 0;

protected:
    int         m_num_inputs;
    int         m_num_outputs;
    unit_type   m_type;
    float**     m_input_buffer;
    float**     m_output_buffer;
    bool        m_is_active;
};
}
}

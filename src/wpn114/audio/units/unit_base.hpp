#pragma once

#include <cstdint>

namespace wpn114 {
namespace audio {

enum unit_type
{
    EFFECT_UNIT,
    GENERATOR_UNIT,
    HYBRID_UNIT
};

class unit_base
{

public:
    virtual ~unit_base();
    virtual void start()            = 0;
    virtual void suspend()          = 0;
    virtual void resume()           = 0;
    virtual void net_expose()       = 0;
    virtual void show()             = 0;
    virtual void initialize_io()    = 0;
    virtual void process_audio()    = 0;

    virtual float get_framedata(uint16_t channel, uint32_t frame) = 0;
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



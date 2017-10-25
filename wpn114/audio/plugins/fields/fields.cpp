#include <math.h>
#include <wpn114/audio/backend/unit_base.hpp>
#include <wpn114/audio/backend/sndfile_support.hpp>

namespace wpn114 {
namespace audio {
namespace plugins {

class fields final : public wpn114::audio::unit_base
{

public:

#ifdef WPN_OSSIA
    void net_expose(std::shared_ptr<ossia::net::node_base> root_node)
    {}
#endif

    fields(const char* sfpath) : m_sf_buffer(0), m_sample_pos(0), m_env_sample_pos(0)
    {
        SETN_INPUTS     (0);
        SETN_OUTPUTS    (2);
        SET_UTYPE       (unit_type::GENERATOR_UNIT);
    }

    void initialize() override
    {
        // initialize envelope

    }

    void process_audio(uint32_t num_frames) override
    {
        // audio processing goes here;
    }

private:
    uint16_t m_sf_buffer;
    uint32_t m_sample_pos;
    float    m_env_sample_pos;
};

}
}
}

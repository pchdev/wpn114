#include <math.h>
#include <wpn114/audio/backend/unit_base.hpp>

class fields final : public wpn114::audio::unit_base
{

public:

#ifdef WPN_OSSIA
    void net_expose(std::shared_ptr<ossia::net::node_base> root_node)
    {}
#endif

    fields::fields() : m_sf_buffer(0), m_sample_pos(0), m_env_sample_pos(0)
    {
        SETN_INPUTS     (0);
        SETN_OUTPUTS    (2);
        SET_UTYPE       (wpn114::audio::GENERATOR_UNIT);
    }

    void fields::process_audio(uint32_t num_frames) override
    {
        // audio processing goes here;
    }

private:
    uint16_t m_sf_buffer;
    uint32_t m_sample_pos;
    float    m_env_sample_pos;
};

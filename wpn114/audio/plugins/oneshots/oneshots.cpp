#include <wpn114/audio/backend/unit_base.hpp>
#include <wpn114/audio/backend/sndfile_support.hpp>

namespace wpn114 {
namespace audio {
namespace plugins {

class oneshots final : public wpn114::audio::unit_base
{

public:

#ifdef WPN_OSSIA

    void net_expose(ossia::net::node_base& root_node)
    {

    }

#endif

    oneshots(const char* sfpath) : m_sf_path(sfpath), m_sample_pos(0)
    {
        SETN_INPUTS     (0);
        SET_UTYPE       (unit_type::GENERATOR_UNIT);

        m_sf_buffer     = load_soundfile<float>(m_sf_path);

        SETN_OUTPUTS    (m_sf_buffer.num_channels);
    }

    void initialize() override {}
    void process_audio(uint32_t num_frames) override
    {
        for(int i = 0; i < num_frames; ++i)
        {
            for(int j = 0; i < N_OUTPUTS; ++i)
            {
                OUT[j][i] = *m_sf_buffer.data++;
            }
        }

    }

private:
    std::string             m_sf_path;
    sndbuf_t<float>         m_sf_buffer;
    uint32_t                m_sample_pos;
};

}
}
}

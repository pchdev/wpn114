#include <wpn114/audio/backend/unit_base.hpp>

namespace wpn114 {
namespace audio {
namespace plugins {

class oneshots final : public wpn114::audio::buffer_unit
{

public:

#ifdef WPN_OSSIA //---------------------------------------------------------------------------------------
    void net_expose(ossia::net::device_base* application_node) override
    {
        auto root       = application_node->get_root_node().create_child(m_name);
        auto play_node  = root->create_child("play");
        auto level_node = root->create_child("level");

        auto play_param     = play_node->create_parameter(ossia::val_type::IMPULSE);
        auto level_param    = level_node->create_parameter(ossia::val_type::FLOAT);

        play_param->add_callback([&](const ossia::value& v) {
            SET_ACTIVE
        });

        level_param->add_callback([&](const ossia::value& v) {
            m_level = v.get<float>();
        });
    }
#endif //-------------------------------------------------------------------------------------------------

    oneshots(const char* name, const char* sfpath, float level) : m_phase(0)
    {
        SET_NAME;
        SET_LEVEL;
        SET_INACTIVE;
        SETN_INPUTS(0);
        SET_UTYPE(unit_type::GENERATOR_UNIT);

        load_soundfile(SFBUF, sfpath);
        SETN_OUTPUTS(m_sf_buffer.num_channels);
    }

    void preprocessing(size_t sample_rate, uint16_t samples_per_buffer) override {}
    void process_audio(float** input, uint16_t samples_per_buffer) override {}
    void process_audio(uint16_t samples_per_buffer) override
    {                
        for(int i = 0; i < samples_per_buffer; ++i)
        {
            if ( m_phase == m_sf_buffer.num_samples )
            {
                // reset buffer, set unit inactive
                // and fill the rest of the buffer with zeroes
                m_sf_buffer.data -= m_sf_buffer.num_frames;
                SET_INACTIVE

                for (int j = 0; j < N_OUTPUTS; ++j)
                    OUT[j][i] = 0.f;
            }
            else if ( m_phase > m_sf_buffer.num_samples )
            {
                // fill the rest of the buffer with zeroes
                for(int j = 0; j < N_OUTPUTS; ++j)
                    OUT[j][i] = 0.f;
            }
            else
                // normal behaviour
            {
                for(int j = 0; j < N_OUTPUTS; ++j)
                    // note: sfbufs are interleaved
                    OUT[j][i] = *m_sf_buffer.data++;
            }

            m_phase++;
        }
    }

    ~oneshots()
    {
        CLEAR_SFBUF;
    }

private:
    uint32_t m_phase;
};
}
}
}

/*
 * =====================================================================================
 *
 *       Filename:  vbap.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  25.10.2017 10:53:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <wpn114/audio/backend/unit_base.hpp>

namespace wpn114 {
namespace audio {
namespace plugins {
//-------------------------------------------------------------------------------------------------------
class rooms final : public wpn114::audio::unit_base
        // designed for audio installations,
        // customizable spatialization interface
        // similar to Reaper's ReaSurround
//-------------------------------------------------------------------------------------------------------
{
public:
    struct rooms_ls
    {
        uint16_t    output_channel;
        float       pos[2];
        float       min[2];
        float       max[2];
        float       radius;
        float       level;
    };

    struct rooms_src
    {
        uint16_t    input_channel;
        float       pos[2];
        float       min[2];
        float       max[2];
        float       radius;
        float       level;
    };

#ifdef WPN_OSSIA //-------------------------------------------------------------------------------------
    void net_expose(ossia::net::device_base* application_node, const char* name) override
    {
        auto root       = application_node->get_root_node().create_child(name);

        auto azimuth_node = root->create_child("azimuth");
        auto level_node = root->create_child("level");

        auto azimuth_param    = azimuth_node->create_parameter(ossia::val_type::FLOAT);
        auto level_param      = level_node->create_parameter(ossia::val_type::FLOAT);

        azimuth_param->add_callback([&](const ossia::value& v) {
            //m_x = v.get<float>();
        });

        level_param->add_callback([&](const ossia::value& v) {
            //m_x = v.get<float>();
        });
    }
#endif //------------------------------------------------------------------------------------------------

    rooms(uint8_t n_inputs, uint8_t n_speakers)
    {
        SET_ACTIVE;
        SETN_INPUTS(    n_inputs);
        SETN_OUTPUTS(   n_speakers);
        SET_UTYPE(      unit_type::EFFECT_UNIT);
    }

    void preprocessing(size_t sample_rate, uint16_t samples_per_buffer) override {}

    inline bool within_ls_area(const rooms_src& src, const rooms_ls& ls)
    {
        return src.pos[0] < ls.max[0] && src.pos[0] > ls.min[0] &&
               src.pos[1] < ls.max[1] && src.pos[1] > ls.min[0];
    }

    inline float compute_speaker_gain(const rooms_src& src, const rooms_ls& ls)
    {
        // compute source position in ls's radius
        float r = (src.pos[0]*src.pos[0]*src.level*ls.level + src.pos[1]*src.pos[1]*src.level*ls.level);
        return r/ls.radius;
    }

    void process_audio(float** input, uint16_t nsamples) override {}
    void process_audio(uint16_t nsamples) override
    {
        for(int i = 0; i < nsamples; ++i)
        {
            for(const auto& src : m_sources)
            {
                for(const auto& ls : m_loudspeakers)
                {
                    if(within_ls_area(src, ls))
                    {
                        // if source is within the ls's radius
                        OUT[ls.output_channel][i] = 0.f;
                                //IN[src.input_channel][i] * compute_speaker_gain(src,ls);
                    }
                    else OUT[ls.output_channel][i] = 0.f;
                }
            }
        }
    }

private:
    std::vector<rooms_ls>   m_loudspeakers;
    std::vector<rooms_src>  m_sources;
};
}
}
}



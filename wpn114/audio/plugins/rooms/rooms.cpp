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
//-------------------------------------------------------------------------------------------------------
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
    struct r_ls
    {
        uint16_t    chn;
        float       pos[2];
        float       min[2];
        float       max[2];
        float       radius;
        float       level;
    };

    struct r_src
    {
        uint16_t    chn;
        float       pos[2];
        float       min[2];
        float       max[2];
        float       radius;
        float       level;
    };

#ifdef WPN_CONTROL_OSSIA //-------------------------------------------------------------------------------------
    void net_expose_plugin_tree(ossia::net::node_base& root) override
    {
        auto speakers           = root.create_child("speakers");
        auto sources            = root.create_child("sources");

        auto speakers_pos       = speakers->create_child("positions");
        auto sources_pos        = sources->create_child("positions");
        auto speakers_lvl       = speakers->create_child("levels");
        auto sources_lvl        = sources->create_child("levels");


    }
#endif //------------------------------------------------------------------------------------------------

    template<typename T> void setup(std::vector<T>& target, uint8_t n)
    {
        for(uint8_t i = 0; i < n; ++i)
        {
            float radius    = 1.f;
            float level     = 1.f;

            T object =   { i,
                         { 0.f, 0.f },
                         { 0.f, 0.f },
                         { 0.f, 0.f },
                         radius, level };

            target.push_back(object);
        }
    }

    rooms(uint8_t n_srcs, uint8_t n_speakers)
    {
        activate();

        SETN_IN     (n_srcs);
        SETN_OUT    (n_speakers);
        SET_UTYPE   (unit_type::EFFECT_UNIT);

        setup<r_src>    (m_sources, n_srcs);
        setup<r_ls>     (m_loudspeakers, n_speakers);
    }

    void preprocess(size_t, uint16_t) override {}

    inline bool within_ls_area(const r_src& src, const r_ls& ls)
    {
        return src.pos[0] < ls.max[0] && src.pos[0] > ls.min[0] &&
               src.pos[1] < ls.max[1] && src.pos[1] > ls.min[0];
    }

    inline float compute_speaker_gain(const r_src& src, const r_ls& ls)
    {
        // compute source position in ls's radius
        float r = (src.pos[0]*src.pos[0]*src.level*ls.level + src.pos[1]*src.pos[1]*src.level*ls.level);
        return r/ls.radius;
    }

    void process(float** input, uint16_t nsamples) override
    {
        auto srcs       = m_sources;
        auto lss        = m_loudspeakers;
        auto out        = OUT;

        for(int i = 0; i < nsamples; ++i)
        {
            for(const auto& src : srcs)
            {
                for(const auto& ls : lss)
                {
                    if(within_ls_area(src, ls))
                    {    
                        out[ls.chn][i] += input[src.chn][i] * compute_speaker_gain(src,ls);
                        // if source is within the ls's radius
                    }
                }
            }
        }
    }

    void process(uint16_t nsamples) override {}

private:
    std::vector<r_ls>   m_loudspeakers;
    std::vector<r_src>  m_sources;
};
}
}
}



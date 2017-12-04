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
    struct r_object
    {
        uint16_t    chn;
        float       pos[2];
        float       min[2];
        float       max[2];
        float       radius;
        float       level;
    };

    inline void update(r_object& obj)
    {
        obj.max[0] = obj.pos[0] + obj.radius;
        obj.max[1] = obj.pos[1] + obj.radius;
        obj.min[0] = obj.pos[0] - obj.radius;
        obj.min[1] = obj.pos[1] - obj.radius;
    }

#ifdef WPN_CONTROL_OSSIA //-------------------------------------------------------------------------------------
    void expose_object_set(ossia::net::node_base& node, std::vector<r_object>& set, std::string setname)
    {
        for(uint8_t i; i < set.size(); ++i)
        {
            auto id             = setname + std::to_string(i);
            auto lsn            = node.create_child(id);
            auto lspn           = lsn->create_child("position");
            auto lsln           = lsn->create_child("level");
            auto lsrn           = lsn->create_child("radius");

            auto lspp           = lspn->create_parameter(ossia::val_type::VEC2F);
            auto lslp           = lsln->create_parameter(ossia::val_type::FLOAT);
            auto lsrp           = lsrn->create_parameter(ossia::val_type::FLOAT);

            lspp->add_callback([=](const ossia::value& v) {
                auto target = set[i];
                auto arr    = v.get<std::array<float,2>>();
                target.pos[0] = arr[0];
                target.pos[1] = arr[1];
                update(target);
            });
        }
    }
    void net_expose_plugin_tree(ossia::net::node_base& root) override
    {
        auto speakers = root.create_child("speakers");
        auto sources = root.create_child("sources");

        expose_object_set(*speakers, m_loudspeakers, "ls_");
        expose_object_set(*sources, m_sources, "src_");
    }
#endif //------------------------------------------------------------------------------------------------

    void setup(std::vector<r_object>& target, uint8_t n)
    {
        for(uint8_t i = 0; i < n; ++i)
        {
            float radius    = 1.f;
            float level     = 1.f;

            r_object object =   { i,
                                { 0.f, 0.f },
                                { 0.f, 0.f },
                                { 0.f, 0.f },
                                radius, level };

            update(object);
            target.push_back(object);
        }
    }

    rooms(uint8_t n_srcs, uint8_t n_speakers)
    {
        activate();

        SETN_IN     (n_srcs);
        SETN_OUT    (n_speakers);
        SET_UTYPE   (unit_type::EFFECT_UNIT);

        setup(m_sources, n_srcs);
        setup(m_loudspeakers, n_speakers);
    }

    void preprocess(size_t, uint16_t) override {}

    inline bool within_ls_area(const r_object& src, const r_object& ls)
    {
        return src.pos[0] < ls.max[0] && src.pos[0] > ls.min[0] &&
               src.pos[1] < ls.max[1] && src.pos[1] > ls.min[0];
    }

    inline float compute_speaker_gain(const r_object& src, const r_object& ls)
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
    std::vector<r_object>   m_loudspeakers;
    std::vector<r_object>   m_sources;
};
}
}
}



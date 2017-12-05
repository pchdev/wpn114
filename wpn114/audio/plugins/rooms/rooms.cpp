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
class rooms2D final : public wpn114::audio::unit_base
        // designed for audio installations,
        // customizable spatialization interface
        // similar to Reaper's ReaSurround
//-------------------------------------------------------------------------------------------------------
{
public:
    struct r_object
    {
        uint16_t    chn;
        float       x;
        float       y;
        float       r;
        float       l;
    };

    enum class setup_type
    {
        STEREO          = 0,
        QUADRAPHONIC    = 1,
        HEXAPHONIC      = 2,
        OCTOPHONIC      = 3,
        FIVE_ONE        = 4,
        SEVEN_ONE       = 5,
        CUSTOM          = 6
    };

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

            lspp->add_callback([&](const ossia::value& v)
            {
                auto target     = set[i];
                auto arr        = v.get<std::array<float,2>>();
                target.x        = arr[0];
                target.y        = arr[1];
            });

            lslp->add_callback([&](const ossia::value& v)
            {
                set[i].l = v.get<float>();
            });

            lsrp->add_callback([&](const ossia::value& v)
            {
                set[i].r = v.get<float>();
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

    inline void setup(std::vector<r_object>& target, uint8_t n)
    {
        for(uint8_t i = 0; i < n; ++i)
        {
            r_object obj = { i, 0.f, 0.f, 1.f, 1.f };
            target.push_back(obj);
        }
    }

    rooms2D(uint8_t n_srcs, uint8_t n_speakers, setup_type stype)
    {
        activate();

        SETN_IN     (n_srcs);
        SETN_OUT    (n_speakers);
        SET_UTYPE   (unit_type::EFFECT_UNIT);

        setup(m_sources, n_srcs);
        setup(m_loudspeakers, n_speakers);
    }

    void preprocess(size_t, uint16_t) override {}

    inline float gain(const r_object& src, const r_object& ls)
    {
        float r = ls.r;

        float dx = abs(ls.x - src.x);
        if (dx > r) return 0.f;
        float dy = abs(ls.y - src.y);
        if (dy > r) return 0.f;

        if     (dx+dy <= r)
        return (dx*dx + dy*dy) *r * ls.l * src.l;
        else   return 0.f;
    }

    void process(float** input, uint16_t nsamples) override
    {
        auto srcs       = m_sources;
        auto lss        = m_loudspeakers;
        auto out        = OUT;

        for(int i = 0; i < nsamples; ++i)
            for (const auto& src : srcs)
                for(const auto& ls : lss)
                    out[ls.chn][i] += input[src.chn][i] * gain(src,ls);
    }

    void process(uint16_t nsamples) override {}

private:
    std::vector<r_object>   m_loudspeakers;
    std::vector<r_object>   m_sources;
};
}
}
}



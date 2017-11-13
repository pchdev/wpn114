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

class circles final : public wpn114::audio::unit_base
{

public:

#ifdef WPN_OSSIA
    void net_expose(ossia::net::device_base* application_node) override
    {
        auto root       = application_node->get_root_node().create_child(m_name);

        auto azimuth_node = root->create_child("azimuth");
        auto level_node = root->create_child("level");

        auto azimuth_param    = azimuth_node->create_parameter(ossia::val_type::FLOAT);
        auto level_param      = level_node->create_parameter(ossia::val_type::FLOAT);

        azimuth_param->add_callback([&](const ossia::value& v) {
            m_x = v.get<float>();
        });

        level_param->add_callback([&](const ossia::value& v) {
            m_x = v.get<float>();
        });
    }
#endif

    circles(const char* name, uint8_t n_inputs, uint8_t n_speakers) :
        m_name(name)
    {
        SET_ACTIVE
        SETN_INPUTS(n_inputs)
        SETN_OUTPUTS(n_speakers)
        SET_UTYPE(unit_type::EFFECT_UNIT)
    }

    void initialize(uint16_t samples_per_buffer) override {}
    void process_audio(uint16_t samples_per_buffer) override
    {
        // compute speaker gains


    }

    inline void atoc2(float r, float th, float& res[2])
    {
        res[0] = r*cos(th);
        res[1] = r*sin(th);
    }

    inline void ctoa2(float x, float y, float& res[2])
    {
        res[0] = sqrt(x*x + y*y);
        res[1] = atan(y/x);
    }

private:
    std::string     m_name;
};
}
}
}



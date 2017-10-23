/*
 * =====================================================================================
 *
 *       Filename:  fields.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  21.10.2017 18:32:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <wpn114/audio/units/unit_base.hpp>
#include <wpn114/audio/soundfiles/soundfile_handler.hpp>
#include <string>

#define ENVSIZE 16384

namespace wpn114 {
namespace audio {
namespace units {

class fields final : public wpn114::audio::unit_base
{

public:
    fields(std::string soundfile_path, uint32_t xfade_length);
    ~fields();

    void start()            override;
    void suspend()          override;
    void resume()           override;
    void net_expose()       override;
    void show()             override;
    void initialize_io()    override;
    void process_audio()    override;

    float get_framedata(uint16_t channel, uint32_t frame) const override;

private:
    sndbuf16_t  m_sf_buffer;
    uint32_t    m_samplepos;
    float       m_env_samplepos;
    uint32_t    m_xfade_length;
    uint32_t    m_xfade_point;
    double      m_env_incr;
    float       m_env[ENVSIZE];
};

}
}
}

#include <wpn114/audio/backend/unit_base.hpp>
#include <math.h>
//-------------------------------------------------------------------------------------------------------
#define ENVSIZE 16384
//-------------------------------------------------------------------------------------------------------
namespace wpn114 {
namespace audio {
namespace plugins {
//-------------------------------------------------------------------------------------------------------
class fields final : public buffer_unit
        // designed for audio installations,
        // simple looping sample-player with crossfading sine envelope
//-------------------------------------------------------------------------------------------------------
{
public:
#ifdef WPN_OSSIA //--------------------------------------------------------------------------------------
    void net_expose_plugin_tree(ossia::net::node_base& root) override {}
#endif //------------------------------------------------------------------------------------------------

    fields(const char* sfpath, uint32_t xfade_length) :
        m_xfade_length(xfade_length),
        m_phase(0.f), m_env_phase(0.f)
    {
        SET_INACTIVE;
        SETN_INPUTS(0);
        SET_UTYPE(unit_type::GENERATOR_UNIT);
        SFLOAD(sfpath);
        SETN_OUTPUTS(SFBUF.num_channels);
    }

    void preprocessing(size_t sample_rate, uint16_t nsamples) override
    {
        (void) sample_rate;
        // initialize crossfade envelope
        m_env_incr      = ENVSIZE/m_xfade_length;
        m_xfade_point   = m_sf_buffer.num_samples - m_xfade_length;

        for (int i = 0; i < ENVSIZE; ++i)
            m_env[i] = sin(i/(float)ENVSIZE*(M_PI_2));
    }

    inline float lininterp(float x, float a, float b)
    { // to be moved in utilities lib
        return a + x * (b - a);
    }

    void process_audio(float** input, uint16_t nsamples)    override {}
    void process_audio(uint16_t frames_per_buffer)          override
    {
        for(int i = 0; i < frames_per_buffer; ++i)
        {
            if(m_phase >= m_xfade_point && m_phase < m_sf_buffer.num_samples)
            {
                //                  if phase is in the crossfade zone
                //                  get data from envelope first (linearly interpolated)
                int y               = floor(m_env_phase);
                float x             = m_env_phase - y;
                float xfade_up      = lininterp(x, m_env[y], m_env[y+1]);
                float xfade_down    = 1-xfade_up;

                for(int j = 0; j < m_sf_buffer.num_channels; ++j)
                {
                    OUT[j][i] = *m_sf_buffer.data++ * xfade_down +
                                *m_sf_buffer.data -m_xfade_point * m_sf_buffer.num_channels * xfade_up;
                }

                m_phase++;
                m_env_phase += m_env_incr;
            }
            else if ( m_phase == m_sf_buffer.num_samples )
            {
                // if phase reaches end of crossfade
                // main phase continues from end of 'up' xfade
                // reset the envelope phase
                m_sf_buffer.data = m_sf_buffer.data + m_xfade_length * N_OUTPUTS - 1;
                for(int j = 0; j < N_OUTPUTS; ++j)
                    OUT[j][i] = *m_sf_buffer.data++;

                m_phase         = m_xfade_length -1;
                m_env_phase     = 0;
            }
            else
            {
                // normal behaviour
                for(int j = 0; j < N_OUTPUTS; ++j)
                    OUT[j][i] = *m_sf_buffer.data++;

                m_phase++;
                // env_phase should be at 0
            }
        }
    }

    ~fields()
    {
        CLEAR_SFBUF;
    }

private:
    uint32_t        m_phase;
    float           m_env_phase;
    uint32_t        m_xfade_length;
    uint32_t        m_xfade_point;
    double          m_env_incr;
    float           m_env[ENVSIZE];
};
}
}
}

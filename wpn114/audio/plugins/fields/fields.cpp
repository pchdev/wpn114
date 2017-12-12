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
private:
    uint32_t        m_phase;
    float           m_env_phase;
    uint32_t        m_xfade_length;
    uint32_t        m_xfade_point;
    double          m_env_incr;
    float           m_env[ENVSIZE];

public:        
    fields(std::string sfpath, uint32_t xfade_length) :
        m_xfade_length(xfade_length),
        m_phase(0.f), m_env_phase(0.f)
    {
        deactivate();

        SET_UTYPE   (unit_type::GENERATOR_UNIT);
        SFLOAD      (sfpath);
        SETN_IN     (0);
        SETN_OUT    (SFBUF.nchannels);
    }

#ifdef WPN_TOML_PARSING
    REGISTER_UNIT_REF("fields")
#endif

    VOID_NET_EXPOSE
    VOID_INPUT_PROCESS

    void preprocess(size_t srate, uint16_t nsamples) override
    {
        (void) srate;
        // initialize crossfade envelope
        m_env_incr      = ENVSIZE/m_xfade_length;
        m_xfade_point   = m_sndbuf.nsamples - m_xfade_length;

        for         (int i = 0; i < ENVSIZE; ++i)
        m_env[i]    = sin(i/(float)ENVSIZE*(M_PI_2));
    }

    inline float lininterp(float x, float a, float b)
    { // to be moved in utilities lib
        return a + x * (b - a);
    }    

    void process(uint16_t frames_per_buffer)          override
    {
        auto xfade_point        = m_xfade_point;
        auto xfade_length       = m_xfade_length;
        auto phase              = m_phase;        
        auto env_phase          = m_env_phase;
        auto env                = m_env;
        auto env_incr           = m_env_incr;
        auto nsamples           = SFBUF.nsamples;
        auto bufdata            = SFBUF.data;
        auto nchannels          = SFBUF.nchannels;
        auto out                = OUT;
        auto n_out              = N_OUT;

        for(int i = 0; i < frames_per_buffer; ++i)
        {
            if(phase >= xfade_point && phase < nsamples)
            {
                //                  if phase is in the crossfade zone
                //                  get data from envelope first (linearly interpolated)
                int y               = floor(env_phase);
                float x             = env_phase - y;
                float xfade_up      = lininterp(x, env[y], env[y+1]);
                float xfade_down    = 1-xfade_up;

                for(int j = 0; j < nchannels; ++j)
                {
                    out[j][i] = *bufdata++ * xfade_down +
                                *bufdata -xfade_point * nchannels * xfade_up;
                }

                phase++;
                env_phase += env_incr;
            }
            else if ( phase == nsamples )
            {
                // if phase reaches end of crossfade
                // main phase continues from end of 'up' xfade
                // reset the envelope phase
                bufdata = bufdata + xfade_length * n_out - 1;

                for         (int j = 0; j < n_out; ++j)
                out[j][i]   = *bufdata++;

                phase         = xfade_length -1;
                env_phase     = 0;
            }
            else
            {
                // normal behaviour
                for         (int j = 0; j < n_out; ++j)
                out[j][i]   = *bufdata++;

                phase++;
                // env_phase should be at 0
            }
        }

        m_phase         = phase;
        m_env_phase     = env_phase;
    }

};
}
}
}

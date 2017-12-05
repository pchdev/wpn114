#include <wpn114/audio/backend/unit_base.hpp>

namespace wpn114 {
namespace audio {
namespace plugins {
//-------------------------------------------------------------------------------------------------------
class oneshots final : public wpn114::audio::buffer_unit
        // one-shot simple sample player
//-------------------------------------------------------------------------------------------------------
{
private:
    uint32_t    m_phase;

public:
#ifdef WPN_CONTROL_OSSIA //---------------------------------------------------------------------------------------
    VOID_NET_EXPOSE
    // nothing to expose, as all required parameters are already taken care of
    // by the 'master' node
#endif //-------------------------------------------------------------------------------------------------

    oneshots(std::string sfpath) : m_phase(0)
    {
        deactivate();

        SET_UTYPE   (unit_type::GENERATOR_UNIT);
        SFLOAD      (sfpath);
        SETN_IN     (0);
        SETN_OUT    (SFBUF.nchannels);
    }

    VOID_PREPROCESS
    VOID_INPUT_PROCESS

    void process(uint16_t samples_per_buffer) override
    {
        auto bufdata        = SFBUF.data;
        auto bufframes      = SFBUF.nframes;
        auto bufsamples     = SFBUF.nsamples;
        auto n_out          = N_OUT;
        auto out            = OUT;
        auto phase          = m_phase;

        // set the phase-copy-pointer back in place
        bufdata += m_phase * SFBUF.nchannels;

        for(int i = 0; i < samples_per_buffer; ++i)
        {
            if ( phase == bufsamples )
            {
                // reset buffer, set unit inactive
                // and fill the rest of the buffer with zeroes
                bufdata -= bufframes;
                deactivate();

                for         (int j = 0; j < n_out; ++j)
                out[j][i]   = 0.f;
            }
            else if ( phase > bufsamples )
            {
                // fill the rest of the buffer with zeroes
                for         (int j = 0; j < n_out; ++j)
                out[j][i]   = 0.f;
            }
            else
                // normal behaviour
            {
                for         (int j = 0; j < n_out; ++j)
                    // note: sfbufs are interleaved
                out[j][i]   = *bufdata++;
            }

            phase++;
        }

        m_phase = phase;
    }
};
}
}
}

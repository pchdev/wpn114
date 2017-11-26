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
#ifdef WPN_OSSIA //---------------------------------------------------------------------------------------
    void net_expose_plugin_tree(ossia::net::node_base& root) override {}
    // nothing to expose, as all required parameters are already taken care of
    // by the 'master' node
#endif //-------------------------------------------------------------------------------------------------

    oneshots(const char* sfpath) : m_phase(0)
    {
        deactivate();

        SET_UTYPE   (unit_type::GENERATOR_UNIT);
        SFLOAD      (sfpath);
        SETN_IN     (0);
        SETN_OUT    (m_sf_buffer.num_channels);
    }

    void preprocessing(size_t sample_rate, uint16_t samples_per_buffer) override {}
    void process_audio(float** input, uint16_t samples_per_buffer) override {}

    void process_audio(uint16_t samples_per_buffer) override
    {
        auto buf_data       = SFBUF.data;
        auto buf_nframes    = SFBUF.nframes;
        auto buf_nsamples   = SFBUF.nsamples;
        auto phase          = m_phase;

        for(int i = 0; i < samples_per_buffer; ++i)
        {
            if ( phase == buf_nsamples )
            {
                // reset buffer, set unit inactive
                // and fill the rest of the buffer with zeroes
                m_sf_buffer.data -= buf_nframes;
                deactivate();

                for         (int j = 0; j < N_OUT; ++j)
                OUT[j][i]   = 0.f;
            }
            else if ( phase > buf_nsamples )
            {
                // fill the rest of the buffer with zeroes
                for         (int j = 0; j < N_OUT; ++j)
                OUT[j][i]   = 0.f;
            }
            else
                // normal behaviour
            {
                for         (int j = 0; j < N_OUT; ++j)
                    // note: sfbufs are interleaved
                OUT[j][i]   = *m_sf_buffer.data++;
            }

            phase++;
        }
    }

    ~oneshots()
    {
        CLEAR_SFBUF;
    }
};
}
}
}

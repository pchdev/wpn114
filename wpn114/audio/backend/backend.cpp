#include <wpn114/audio/backend/backend.hpp>
#include <iostream>

using namespace wpn114::audio;

int main_stream_callback
(const void *input_buffer, void *output_buffer, unsigned long frames_per_buffer,
 const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void *user_data)
{
    float*          out     = (float*) output_buffer;
    backend_hdl*    backend = (backend_hdl*) user_data;

    (void) time_info;
    (void) status_flags;
    (void) input_buffer;

    // copy unit's configuration
    std::vector<unit_base*> units = backend->get_registered_units();
    uint8_t nchannels = backend->get_num_channels();
    float frame_data[frames_per_buffer];

    // call audio processing on each of the registered units
    for(auto& unit : units)
    {
        unit->process_audio(frames_per_buffer);
    }

    for(uint32_t i = 0; i < frames_per_buffer; ++i)
    {
        for(auto& unit : units)
        {
            if(unit->is_active())
            {
                for(uint8_t n = 0; n < unit->get_num_channels(); n++)
                {
                    frame_data[i] += unit->get_framedata(n, i);
                }
            }
        }

        *out++ = frame_data[i];
    }

    return paContinue;
}

wpn114::audio::backend_hdl::backend_hdl(uint8_t num_channels) :
    m_main_stream(nullptr),
    m_num_channels(num_channels) {}

wpn114::audio::backend_hdl::~backend_hdl()
{
    PaError err = Pa_StopStream(m_main_stream);
            err = Pa_CloseStream(m_main_stream);

    Pa_Terminate();
}

inline std::vector<wpn114::audio::unit_base*>
wpn114::audio::backend_hdl::get_registered_units() const
{
    return m_registered_units;
}

inline uint8_t backend_hdl::get_num_channels() const
{
    return m_num_channels;
}

void wpn114::audio::backend_hdl::register_unit(wpn114::audio::unit_base* unit)
{
    m_registered_units.push_back(unit);
}

void wpn114::audio::backend_hdl::unregister_unit(wpn114::audio::unit_base* unit)
{
    m_registered_units.erase(
                std::remove(
                    m_registered_units.begin(), m_registered_units.end(), unit),
                m_registered_units.end());
    /*for     (auto it = m_registered_units.begin(); it != m_registered_units.end(); ++it)
    if      (it == unit)
            m_registered_units.erase(it);
            */
}

void wpn114::audio::backend_hdl::initialize()
{
    PaError err;

    // dont't forget error management
    err = Pa_Initialize();

    m_output_parameters.device                      = Pa_GetDefaultOutputDevice();
    m_output_parameters.channelCount                = m_num_channels;
    m_output_parameters.sampleFormat                = paFloat32;
    m_output_parameters.hostApiSpecificStreamInfo   = NULL;
    m_main_stream_cb_funcptr                        = &main_stream_callback;
    m_output_parameters.suggestedLatency            = Pa_GetDeviceInfo
                                                      (m_output_parameters.device)->defaultLowOutputLatency;

    for(auto& unit : m_registered_units)
    {
        // initialize registered units
        unit->initialize_io(context.blocksize);
        unit->initialize(context.blocksize);
    }
}

void wpn114::audio::backend_hdl::start_stream()
{
    PaError err = Pa_OpenStream(
                        &m_main_stream,
                        NULL,
                        &m_output_parameters,
                        wpn114::audio::context.sample_rate,
                        wpn114::audio::context.blocksize,
                        paClipOff,
                        m_main_stream_cb_funcptr,
                        this );
}

void wpn114::audio::backend_hdl::stop_stream()
{
    PaError err = Pa_StopStream(m_main_stream);
}

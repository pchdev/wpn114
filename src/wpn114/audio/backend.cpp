#include <wpn114/audio/backend.hpp>

#include <iostream>

using namespace wpn114::audio;

int main_stream_callback
(const void *input_buffer, void *output_buffer, unsigned long frames_per_buffer,
 const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void *user_data)
{
    float*                  out     = (float*) output_buffer;
    wpn114::audio::backend* backend = (wpn114::audio::backend*) user_data;

    (void)                  time_info;
    (void)                  status_flags;
    (void)                  input_buffer;

    unsigned long           i;

    for(auto& unit : backend->get_registered_units())
    {
        unit->process_audio();
    }

    //! TODO: for each audio channel, important when we'll have ambisonics and vbap..
    for(i = 0; i < frames_per_buffer; ++i)
    {
        float frame_data_l = 0;
        float frame_data_r = 0;

        for(auto& unit : backend->get_registered_units())
        {
            frame_data_l += unit->get_framedata(0, i);
            frame_data_r += unit->get_framedata(0, i);
        }

        *out++ = frame_data_l;
        *out++ = frame_data_r;
    }

    return paContinue;
}

wpn114::audio::backend::backend(uint16_t num_channels) :
    m_main_stream(nullptr),
    m_num_channels(num_channels) {}

wpn114::audio::backend::~backend()
{
    PaError err = Pa_StopStream(m_main_stream);
            err = Pa_CloseStream(m_main_stream);

    Pa_Terminate();
}

std::vector<wpn114::audio::unit_base*>
wpn114::audio::backend::get_registered_units() const
{
    return m_registered_units;
}

void wpn114::audio::backend::register_unit(wpn114::audio::unit_base* unit)
{
    m_registered_units.push_back(unit);
}

void wpn114::audio::backend::unregister_unit(wpn114::audio::unit_base* unit)
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

void wpn114::audio::backend::initialize_io()
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
        unit->initialize_io();
}

void wpn114::audio::backend::start_stream()
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

void wpn114::audio::backend::stop_stream()
{
    PaError err = Pa_StopStream(m_main_stream);
}

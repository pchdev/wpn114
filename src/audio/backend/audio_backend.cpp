#include "audio_backend.hpp"

#include <iostream>

using namespace wpn114::audio::backend;

wpn114::audio::backend::backend_handler::backend_handler(int num_channels) :
    m_main_stream(nullptr)
{
     PaError err;

     // dont't forget error management
     err = Pa_Initialize();

     m_output_parameters.device = Pa_GetDefaultOutputDevice();
     m_output_parameters.channelCount = num_channels;
     m_output_parameters.sampleFormat = paFloat32;
     m_output_parameters.suggestedLatency = Pa_GetDeviceInfo(m_output_parameters.device)->defaultLowOutputLatency;
     m_output_parameters.hostApiSpecificStreamInfo = NULL;
}

wpn114::audio::backend::backend_handler::~backend_handler()
{
    PaError err = Pa_StopStream(m_main_stream);
    err = Pa_CloseStream(m_main_stream);
    Pa_Terminate();
}

void wpn114::audio::backend::backend_handler::register_callback()
{

}

int wpn114::audio::backend::backend_handler::m_main_stream_callback
(const void *input_buffer, void *output_buffer, unsigned long frames_per_buffer,
 const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void *user_data)
{
    float* out = (float*)output_buffer;
    unsigned long i;

    (void) time_info;
    (void) status_flags;
    (void) input_buffer;

    for(auto& unit : m_registered_units)
    {
        unit->process_audio(frames_per_buffer);
    }

    //! TODO: for each audio channel, important when we'll have ambisonics and vbap..
    for(i = 0; i < frames_per_buffer; ++i)
    {
        float frame_data_l = 0;
        float frame_data_r = 0;

        for(auto& unit : m_registered_units)
        {
            frame_data_l += unit->get_framedata(0, i);
            frame_data_r += unit->get_framedata(0, i);
        }

        *out++ = frame_data_l;
        *out++ = frame_data_r;
    }

    return paContinue;
}

void wpn114::audio::backend::backend_handler::start_stream(long sample_rate, int frames_per_buffer)
{
    PaError err = Pa_OpenStream(&m_main_stream,
                        NULL,
                        &m_output_parameters,
                        sample_rate,
                        frames_per_buffer,
                        paClipOff,
                        m_main_stream_callback,
                        NULL );
}

void wpn114::audio::backend::backend_handler::stop_stream()
{
    PaError err = Pa_StopStream(m_main_stream);
}

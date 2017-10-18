#include "backend.hpp"

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

void wpn114::audio::backend::backend_handler::start_stream(long sample_rate, int frames_per_buffer)
{
    PaError err = Pa_OpenStream(&m_main_stream,
                        NULL,
                        &m_output_parameters,
                        sample_rate,
                        frames_per_buffer,
                        paClipOff,
                        NULL,
                        NULL );
}

void wpn114::audio::backend::backend_handler::stop_stream()
{
    PaError err = Pa_StopStream(m_main_stream);
}

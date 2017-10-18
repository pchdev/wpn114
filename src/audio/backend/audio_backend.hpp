/*
 * =====================================================================================
 *
 *       Filename:  backend.h
 *
 *    Description:  for now, manages audio output only...
 *
 *        Version:  0.1
 *        Created:  15.10.2017 17:46:25
 *       Revision:  none
 *       Compiler:  clang++
 *
 *         Author:  ll-drs
 *   Organization:  cerr
 *
 * =====================================================================================
 */

#pragma once
#include <portaudio.h>
#include <vector>

namespace wpn114
{
namespace audio
{
namespace backend
{

typedef float *(*seccallback)(float*input_buffer);

class backend_handler
{
public:
    backend_handler(int num_channels);
    ~backend_handler();
    void start_stream(long sample_rate, int frames_per_buffer);
    void stop_stream();
    void register_callback(seccallback callback);
    void unregister_callback(seccallback callback);

private:
    PaStream* m_main_stream;
    PaStreamParameters m_output_parameters;
    int m_main_stream_callback(const void *input_buffer, void *output_buffer,
                        unsigned long frames_per_buffer,
                        const PaStreamCallbackTimeInfo* time_info,
                        PaStreamCallbackFlags status_flags,
                        void *user_data);

    std::vector<seccallback> m_registered_stream_callbacks;

};

}
}
}

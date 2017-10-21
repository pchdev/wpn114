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

#include <vector>
#include <portaudio.h>
#include <wpn114/audio/units/unit_base.hpp>

namespace wpn114
{
namespace audio
{
class backend
{
public:
    backend(int num_channels = 2);
    ~backend();
    void start_stream(long sample_rate, int frames_per_buffer);
    void stop_stream();
    void register_unit(wpn114::audio::unit_base* unit);
    void unregister_unit(wpn114::audio::unit_base* unit);
    std::vector<wpn114::audio::unit_base*> get_registered_units() const;

private:
    PaStream* m_main_stream;
    PaStreamParameters m_output_parameters;
    std::vector<wpn114::audio::unit_base*> m_registered_units;
    PaStreamCallback* m_main_stream_cb_funcptr;
};

}
}

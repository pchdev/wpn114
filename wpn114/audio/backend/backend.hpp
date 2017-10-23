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
#include <wpn114/audio/backend/unit_base.hpp>
#include <wpn114/audio/backend/context.hpp>

namespace wpn114
{
namespace audio
{
class backend
{
public:
    backend(uint16_t num_channels = 2);
    ~backend();
    void initialize_io();
    void start_stream();
    void stop_stream();
    void register_unit(wpn114::audio::unit_base* unit);
    void unregister_unit(wpn114::audio::unit_base* unit);
    std::vector<wpn114::audio::unit_base*> get_registered_units() const;

private:
    uint16_t            m_num_channels;
    PaStream*           m_main_stream;
    PaStreamParameters  m_output_parameters;
    PaStreamCallback*   m_main_stream_cb_funcptr;
    std::vector<wpn114::audio::unit_base*> m_registered_units;    
};

}
}

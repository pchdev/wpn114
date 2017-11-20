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
class backend_hdl
{
public:
    backend_hdl(uint8_t num_channels = 2);
    ~backend_hdl();

    void initialize(size_t sample_rate, uint16_t nsamples);
    void start_stream(size_t sample_rate, uint16_t nsamples);
    void stop_stream();
    void bufalloc(uint16_t nsamples);

    void register_unit(unit_base* unit);
    void unregister_unit(unit_base* unit);

    uint8_t                     get_num_channels() const;
    std::vector<unit_base*>     get_registered_units() const;
    float***                    get_master_output_buffer();

private:
    uint8_t                     m_num_channels;
    PaStream*                   m_main_stream;
    PaStreamParameters          m_output_parameters;
    PaStreamCallback*           m_main_stream_cb_funcptr;
    std::vector<unit_base*>     m_units;
    float**                     m_master_output;
};
}
}

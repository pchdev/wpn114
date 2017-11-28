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
//-------------------------------------------------------------------------------------------------------
#include <vector>
#include <portaudio.h>
#include <wpn114/audio/backend/unit_base.hpp>
#include <wpn114/audio/backend/context.hpp>
//-------------------------------------------------------------------------------------------------------
using namespace std;
//-------------------------------------------------------------------------------------------------------
namespace wpn114
{
namespace audio
{
//-------------------------------------------------------------------------------------------------------
class backend_hdl
//-------------------------------------------------------------------------------------------------------
{
public:
//-------------------------------------------------------------------------------------------------------
    backend_hdl(uint8_t nchannels = 2);
    ~backend_hdl();
//-------------------------------------------------------------------------------------------------------
    void                start(size_t srate, uint16_t nsamples);
    void                stop();
    void                initialize(size_t srate, uint16_t nsamples);
    void                bufalloc(uint16_t nsamples);
    void                register_unit(unit_base* unit);
    void                unregister_unit(unit_base* unit);
//-------------------------------------------------------------------------------------------------------
    uint8_t             nchannels()     const;
    vector<unit_base*>  units()         const;
//-------------------------------------------------------------------------------------------------------
private:
    float***                    out()   const;
//-------------------------------------------------------------------------------------------------------
    uint8_t                     m_nchannels;
    PaStream*                   m_stream;
    PaStreamParameters          m_outparameters;
    PaStreamCallback*           m_main_stream_cb_funcptr;
    std::vector<unit_base*>     m_units;
    float**                     m_out;
};
//-------------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& unit, const backend_hdl& hdl);
std::ostream& operator>>(std::ostream& unit, const backend_hdl& hdl);
//-------------------------------------------------------------------------------------------------------
}
}

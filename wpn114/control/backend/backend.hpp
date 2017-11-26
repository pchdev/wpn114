/*
 * =====================================================================================
 *
 *       Filename:  midi_backend.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  18.10.2017 21:38:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#pragma once
//-------------------------------------------------------------------------------------------------------
#include <wpn114/control/backend/controller_base.hpp>
#include <string>
#include <RtMidi.h>
//-------------------------------------------------------------------------------------------------------
namespace wpn114 {
namespace control {
//-------------------------------------------------------------------------------------------------------
enum class device_io_type;
class controller_base;
//-------------------------------------------------------------------------------------------------------
namespace midi {
using rtmidiin  = RtMidiIn;
using rtmidiout = RtMidiOut;
//-------------------------------------------------------------------------------------------------------
enum class commands
{
    SYSEX                   = 0xf0,
    EOX                     = 0xf7,
    NOTE_OFF                = 0x80,
    NOTE_ON                 = 0x90,
    AFTERTOUCH              = 0xa0,
    CONTINUOUS_CONTROL      = 0xb0,
    PATCH_CHANGE            = 0xc0,
    CHANNEL_PRESSURE        = 0xd0,
    PITCH_BEND              = 0xe0
};
//-------------------------------------------------------------------------------------------------------
class device_hdl
//-------------------------------------------------------------------------------------------------------
{
public:
    device_hdl(rtmidiin* input);
    device_hdl(rtmidiout* output);
    device_hdl(rtmidiin* input, rtmidiout* output);
    device_hdl(const device_hdl&)   = delete;
    device_hdl(device_hdl&&)        = delete;
    ~device_hdl();
//-------------------------------------------------------------------------------------------------------
    void enable_reception();
    void disable_reception();
    bool is_enabled() const;
    void add_callback(void (*callback_function)(double, std::vector<uint8_t>&, void*));
    device_io_type get_io_type() const;
//-------------------------------------------------------------------------------------------------------
    void out(commands command_id, uint8_t channel, uint8_t index, uint8_t value) const;
    void sysxout(const std::vector<uint8_t> &byte_array) const;
//-------------------------------------------------------------------------------------------------------
    template<typename T>  static void   print_all_ports() noexcept;
    template<typename T>  static int    get_port_index(std::string& target_refname) noexcept;
//-------------------------------------------------------------------------------------------------------
private:
    bool            m_is_enabled;
    rtmidiin*       m_midi_input;
    rtmidiout*      m_midi_output;
    device_io_type  m_io_type;
};
//-------------------------------------------------------------------------------------------------------
typedef std::unique_ptr<wpn114::control::midi::device_hdl> unique_device_hdl;
//-------------------------------------------------------------------------------------------------------
class device_factory
//-------------------------------------------------------------------------------------------------------
{
public:
    device_factory  ();
    ~device_factory ();
//-------------------------------------------------------------------------------------------------------
    unique_device_hdl make_device_hdl(std::string& controller_id, device_io_type io_type);
    void emplace_device_hdl(controller_base& target, device_io_type io_type);
//-------------------------------------------------------------------------------------------------------
private:
    template<typename T> T* open_port(std::string& controller_id) noexcept;
};
}
}
}

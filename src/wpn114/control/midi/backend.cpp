/*
 * =====================================================================================
 *
 *       Filename:  midi_backend.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  18.10.2017 21:39:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <wpn114/control/midi/backend.hpp>

using namespace wpn114::control::midi;

template<typename T> void print_all_midi_ports()
{
    T rt_port_hdl;
    auto num_ports = rt_port_hdl.getPortCount();
    std::string port_name;

    for(uint16_t i = 0; i < num_ports; ++i)
    {
        port_name = rt_port_hdl.getPortName(i);
        std::cout << port_name << std::endl;
    }
}

template<typename T> int get_index_for_port(std::string& port_reference_target)
{
    T rt_port_hdl;
    uint16_t num_ports = rt_port_hdl.getPortCount();

    for(uint16_t i = 0; i < num_ports; ++i)
        if(port_reference_target == rt_port.getPortName(i))
            return i;

    return -1;
}
template<typename T> T* device_factory::open_port(std::string& controller_port_reference)
{
    T* res_port = new T();
    auto port_index = get_index_for_port<T>(controller_port_reference);
    res_port->openPort(port_index);
    return res_port;
}

device_factory::device_factory()  {}
device_factory::~device_factory() {}

void device_factory::make_device_hdl (controller_base &target_controller, device_io_type io_type)
{
}

unique_device_hdl make_device_hdl(std::string &controller_port_reference, device_io_type io_type)
{
    switch(io_type)
    {
    case device_io_type::INPUT:
        auto input_hdl = open_port<rtmidiin>(controller_port_reference);
        return std::make_unique<device_hdl>(input_hdl);
        break;

    case device_io_type::OUTPUT:
        auto output_hdl = open_port<rtmidiout>(controller_port_reference);
        return std::make_unique<device_hdl>(output_hdl);
        break;

    case device_io_type::IN_OUT:
        auto input_hdl = open_port<rtmidiin>(controller_port_reference);
        auto output_hdl = open_port<rtmidiout>(controller_port_reference);
        return std::make_unique<device_hdl>(input_hdl, output_hdl);
        break;
    }
}

device_hdl::device_hdl(rtmidiin* input) : m_midi_input(input) {}
device_hdl::device_hdl(rtmidiout *output) : m_midi_output(output) {}
device_hdl::device_hdl(rtmidiin *input, rtmidiout *output) : m_midi_input(input), m_midi_output(output) {}

device_hdl::~device_hdl()
{
    if(m_midi_input)    delete m_midi_input;
    if(m_midi_output)   delete m_midi_output;
}

void device_hdl::enable_reception() { m_is_enabled = true; }
void device_hdl::disable_reception() { m_is_enabled = false; }
void device_hdl::is_enabled() { return m_is_enabled; }


inline void device_hdl::send_note_on(uint8_t channel, uint8_t index, uint8_t value) const
{
    std::vector<uint8_t> output = { NOTE_ON + channel, index, value };
    m_midi_output->sendMessage(&output);
}

inline void device_hdl::send_note_off(uint8_t channel, uint8_t index, uint8_t value) const
{
    std::vector<uint8_t> output = { NOTE_OFF + channel, index, value };
    m_midi_output->sendMessage(&output);
}

inline void device_hdl::send_control_change(uint8_t channel, uint8_t index, uint8_t value) const
{
    std::vector<uint8_t> output = { CONTINUOUS_CONTROL + channel, index, value };
    m_midi_output->sendMessage(&output);
}

void device_hdl::send_program_change(uint8_t channel, uint8_t index, uint8_t value) const
{
    std::vector<uint8_t> output = { PATCH_CHANGE + channel, index, value };
    m_midi_output->sendMessage(&output);
}

inline void device_hdl::send_sysex(std::vector<uint8_t> byte_array) const
{
    m_midi_output->sendMessage(&byte_array);
}

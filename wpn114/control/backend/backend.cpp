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

#include <wpn114/control/backend/backend.hpp>

using namespace wpn114::control::midi;

template<typename T> void device_hdl::print_all_ports() noexcept
{
    T               rt_port_hdl;
    auto            num_ports = rt_port_hdl.getPortCount();

    for(uint16_t i = 0; i < num_ports; ++i)
    {
        std::cout << rt_port_hdl.getPortName(i) << std::endl;
    }
}

template<typename T> int device_hdl::get_port_index(std::string& port_id) noexcept
{
    T           rt_port_hdl;
    uint16_t    num_ports = rt_port_hdl.getPortCount();

    for(uint16_t i = 0; i < num_ports; ++i)
        if      (port_id == rt_port_hdl.getPortName(i))
        return  i;

    return      -1;
}

template<typename T> T* device_factory::open_port(std::string& port_id) noexcept
{
    T* res_port         = new T();
    auto port_index     = device_hdl::get_port_index<T>(port_id);
    res_port            ->openPort(port_index);
    return              res_port;
}

device_factory::device_factory()  {}
device_factory::~device_factory() {}

void device_factory::emplace_device_hdl(controller_base &target_controller, device_io_type io_type)
{
}

using namespace wpn114::control;

unique_device_hdl device_factory::make_device_hdl(std::string &controller_port_reference, device_io_type io_type)
{
    switch(io_type)
    {
    case device_io_type::INPUT:
    {
        auto input_hdl = open_port<rtmidiin>(controller_port_reference);
        return std::make_unique<device_hdl>(input_hdl);
    }
    case device_io_type::OUTPUT:
    {
        auto output_hdl = open_port<rtmidiout>(controller_port_reference);
        return std::make_unique<device_hdl>(output_hdl);
    }
    case device_io_type::IN_OUT:
    {
        auto input_hdl  = open_port<rtmidiin>(controller_port_reference);
        auto output_hdl = open_port<rtmidiout>(controller_port_reference);
        return std::make_unique<device_hdl>(input_hdl, output_hdl);
    }
    }
}

device_hdl::device_hdl(rtmidiin* input)     : m_midi_input(input)       {}
device_hdl::device_hdl(rtmidiout *output)   : m_midi_output(output)     {}

device_hdl::device_hdl(rtmidiin *input, rtmidiout *output) :
    m_midi_input(input),
    m_midi_output(output)
{}

device_hdl::~device_hdl()
{
    if(m_midi_input)    delete m_midi_input;
    if(m_midi_output)   delete m_midi_output;
}

using namespace std;

void device_hdl::enable_reception()     { m_is_enabled = true; }
void device_hdl::disable_reception()    { m_is_enabled = false; }
bool device_hdl::is_enabled() const     { return m_is_enabled; }

void device_hdl::out(commands command_id, uint8_t channel, uint8_t index, uint8_t value) const
{
    vector<uint8_t> output =
    {
        static_cast<uint8_t>((uint8_t) command_id + channel), index, value
    };

    m_midi_output->sendMessage(&output);
}

void device_hdl::sysxout(const std::vector<uint8_t>& byte_array) const
{
    m_midi_output->sendMessage(&byte_array);
}

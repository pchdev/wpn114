/*
 * =====================================================================================
 *
 *       Filename:  push_1.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  21.10.2017 19:51:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>

#include <wpn114/control/plugins/push_1/push_controller.hpp>

using namespace wpn114::control::midi;

push_1::push_1(unique_device_hdl hdl) :
    m_device_hdl(std::move(hdl))
{
    this->light_pad(0, pad_colors::BLUE_GREEN, pad_lighting_mode::FLASH_EIGHTH_BEATS);
}

push_1::~push_1()
{
    //m_device_hdl->stop();
    m_device_hdl.reset();
}

std::string push_1::get_controller_id() const
{
    return (std::string) "Ableton Push User Port";
}

void push_1::light_pad(uint8_t target, pad_colors color, pad_lighting_mode mode)
{
    m_device_hdl->send_note_on(static_cast<uint8_t>(mode), target, static_cast<uint8_t>(color));
}

void push_1::light_cc_commmand_button(command_buttons target, toggle_lighting_mode mode)
{
    m_device_hdl->send_control_change(0, static_cast<uint8_t>(target), static_cast<uint8_t>(mode));
}

void push_1::light_cc_toggle(uint8_t row, uint8_t target, toggle_lighting_mode mode)
{
    uint8_t offset = 0;
    if (row == 1) offset = 19;
    else offset = 101;

    m_device_hdl->send_control_change(1, target+offset, static_cast<uint8_t>(mode));
}

void push_1::lcd_display (std::string text_to_display, uint8_t line, uint8_t divide, uint8_t slot, lcd_alignment align)
{

}


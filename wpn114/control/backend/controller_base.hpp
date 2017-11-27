/*
 * =====================================================================================
 *
 *       Filename:  controller_base.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  18.10.2017 21:39:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#pragma once
#include <wpn114/control/backend/backend.hpp>
#include <string>
//-------------------------------------------------------------------------------------------------------
namespace wpn114 {
namespace control {
//-------------------------------------------------------------------------------------------------------
enum class protocol
{
    MIDI    = 0,
    OSC     = 1,
    SERIAL  = 2,
    WS      = 3,
    HTTP    = 4
};
//-------------------------------------------------------------------------------------------------------
enum class device_io_type
{
    INPUT   = 0,
    OUTPUT  = 1,
    IN_OUT  = 2
};
//-------------------------------------------------------------------------------------------------------
class controller_base        
//-------------------------------------------------------------------------------------------------------
{
public:
    virtual ~controller_base() {}
    std::string get_controller_id() const;
    device_io_type get_controller_io_type() const;

protected:
    std::string           m_controller_id;
    unique_device_hdl     m_device_hdl;
};
//-------------------------------------------------------------------------------------------------------
}
}

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

#include <wpn114/control/midi/backend.hpp>

namespace wpn114 {
namespace control {

enum controller_protocol
{
    MIDI    = 0,
    OSC     = 1
};

enum device_io_type
{
    INPUT   = 0,
    OUTPUT  = 1,
    IN_OUT  = 2
};

class controller_base        
{
public:
    virtual ~controller_base();
    virtual std::string get_controller_id() const = 0;

protected:
    std::string m_controller_id;
};

}
}

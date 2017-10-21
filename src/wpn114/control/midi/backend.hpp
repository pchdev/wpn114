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

#include <portmidi.h>
#include <string>

namespace wpn114 {
namespace midi {

enum commands
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

class backend
{
    backend(std::string device);
    ~backend();
    void enable_reception();
    void disable_reception();
    void send_note_on(uint8_t channel, uint8_t index, uint8_t value);
    void send_note_off(uint8_t channel, uint8_t index, uint8_t value);
    void send_control_change(uint8_t channel, uint8_t index, uint8_t value);
    void send_program_change(uint8_t channel, uint8_t index, uint8_t value);
    void send_sysex(std::vector<uint8_t> byte_array);

};
}
}

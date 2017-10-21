/*
 * =====================================================================================
 *
 *       Filename:  push_controller.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  21.10.2017 19:51:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include <wpn114/control/controller_base.hpp>
#include <cstdint>
#include <string>

namespace wpn114 {
namespace control {

class push_controller final : public controller_base
{
    enum command_buttons
    {
        PLAY                    = 85,
        RECORD                  = 86,
        NEW                     = 87,
        DUPLICATE               = 88,
        AUTOMATION              = 89,
        FIXED_LENGTH            = 90,
        QUANTIZE                = 116,
        DOUBLE                  = 117,
        DELETE                  = 118,
        UNDO                    = 119,
        METRONOME               = 9,
        TAP_TEMPO               = 3,
        LEFT_ARROW              = 44,
        RIGHT_ARROW             = 45,
        UP_ARROW                = 46,
        DOWN_ARROW              = 47,
        SELECT                  = 48,
        SHIFT                   = 49,
        NOTE                    = 50,
        SESSION                 = 51,
        ADD_EFFECT              = 52,
        ADD_TRACK               = 53,
        OCTAVE_DOWN             = 54,
        OCTAVE_UP               = 55,
        REPEAT                  = 56,
        ACCENT                  = 57,
        SCALES                  = 58,
        USER                    = 59,
        MUTE                    = 60,
        SOLO                    = 61,
        NEXT                    = 62,
        PREVIOUS                = 63,
        DEVICE                  = 110,
        BROWSE                  = 111,
        TRACK                   = 112,
        CLIP                    = 113,
        VOLUME                  = 114,
        PAN_SEND                = 115,
        FOURTH                  = 36,
        FOURTH_T                = 37,
        EIGHTH                  = 38,
        EIGHTH_T                = 39,
        SIXTEENTH               = 40,
        SXITEENTH_T             = 41,
        THIRTY_SECOND           = 42,
        THIRTY_SECOND_T         = 43,
        MASTER                  = 28,
        STOP                    = 29
    };

    enum toggle_lighting_mode
    {
        OFF                     = 0,
        RED_DIM                 = 1,
        RED_DIM_SLOW            = 2,
        RED_DIM_FAST            = 3,
        RED_FULL                = 4,
        RED_FULL_SLOW           = 5,
        RED_FULL_FAST           = 6,
        ORANGE_DIM              = 7,
        ORANGE_DIM_SLOW         = 8,
        ORANGE_DIM_FAST         = 9,
        ORANGE_FULL             = 10,
        ORANGE_FULL_SLOW        = 11,
        ORANGE_FULL_FAST        = 12,
        YELLOW_DIM              = 13,
        YELLOW_DIM_SLOW         = 14,
        YELLOW_DIM_FAST         = 15,
        YELLOW_FULL             = 16,
        YELLOW_FULL_SLOW        = 17,
        YELLOW_FULL_FAST        = 18,
        GREEN_DIM               = 19,
        GREEN_DIM_SLOW          = 20,
        GREEN_DIM_FAST          = 21,
        GREEN_FULL              = 22,
        GREEN_FULL_SLOW         = 23,
        GREEN_FULL_FAST         = 24
    };

    enum pad_lighting_mode
    {
        OFF                     = 0,
        NORMAL                  = 1,
        FADE_IN_FAST            = 2,
        FADE_IN_MED             = 3,
        FADE_IN_SLOW            = 4,
        FLASH_TRIPLET_BEATS     = 7,
        CYCLE_COLORS_FAST       = 8,
        PULSE_QUARTER_BEATS     = 9,
        PULSE_WHOLE_BEATS       = 10,
        PULSE_TWO_BEATS         = 11,
        FLASH_TRIPLET_BEATS     = 12,
        FLASH_EIGHTH_BEATS      = 13,
        FLASH_QUARTER_BEATS     = 14,
        FLASH_WHOLE_BEATS       = 15,
        FLASH_TWO_BEATS         = 16
    };

    enum pad_colors
    {
        BLACK                   = 0,
        LIGHTEST_WHITE          = 1,
        LIGHTER_WHITE           = 2,
        WHITE                   = 3,
        SALMON                  = 4,
        RED                     = 5,
        DARKER_RED              = 6,
        DARKEST_RED             = 7,
        BEIGE                   = 8,
        ORANGE                  = 9,
        DARKER_ORANGE           = 10,
        DARKEST_ORANGE          = 11,
        GOLD                    = 12,
        YELLOW                  = 13,
        DARKER_YELLOW           = 14,
        DARKEST_YELLOW          = 15,
        APPLE_GREEN             = 16,
        GRASS_GREEN             = 17,
        DARKER_GRASS_GREEN      = 18,
        DARKEST_GRASS_GREEN     = 19,
        LIGHT_BLUE_GREEN        = 20,
        GREEN                   = 21,
        DARKER_GREEN            = 22,
        DARKEST_GREEN           = 23,
        SEA_GREEN               = 24,
        BLUE_GREEN              = 25,
        GLACIER_GREEN           = 26,
        DARK_BLUE_GREEN         = 27,
        BLUE_STEEL              = 28,
        BLUE_GREEN_2            = 29,
        DARK_BLUE_GREEN_2       = 30,
        DARKEST_BLUE_GREEN      = 31,
        BLUE_STEEL_2            = 32,
        FLUO_BLUE               = 33,
        DARK_BLUE_GREEN_3       = 34,
        DARKEST_BLUE_GREEN_2    = 35,
        LIGHT_SKY_BLUE          = 36,
        SKY_BLUE                = 37,
        DARK_SKY_BLUE           = 38,
        NIGHT_SKY_BLUE          = 39,
        SUNSET_BLUE             = 40,
        SKY_BLUE_2              = 41,
        DARK_SKY_BLUE_2         = 42,
        NIGHT_SKY_BLUE_2        = 43,
        LIGHT_PURPLE            = 44,
        PURPLISH_BLUE           = 45,
        DARK_PURPLISH_BLUE      = 46,
        DARKEST_PURPLISH_BLUE   = 47,
        LIGHT_PURPLE_2          = 48,
        PURPLE                  = 49,
        DARK_PURPLE             = 50,
        DARKEST_PURPLE          = 51,
        LIGHT_FUSCIA            = 52,
        FUSCIA                  = 53,
        DARK_FUSCIA             = 54,
        DARKEST_FUSCIA          = 55,
        LIPS_PINK               = 56,
        PINK                    = 57,
        DARK_PINK               = 58,
        DARKEST_PINK            = 59,
        ORANGE_CANDY            = 60,
        ORANGE_MANDARINE        = 61,
        ORANGE_MANGO            = 62,
        YELLOW_GREEN            = 63,
        DARK_GREEN              = 64
    };

    push_controller();
    ~push_controller();

    void light_pad(uint8_t target, pad_colors color, pad_lighting_mode mode);
    void light_cc_commmand_button(command_buttons target, toggle_lighting_mode mode);
    void light_cc_toggle(uint8_t target, toggle_lighting_mode mode);
    void lcd_display(std::string text_to_display);

};

}
}

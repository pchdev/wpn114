#ifndef DEVICE_ENUMS_HPP
#define DEVICE_ENUMS_HPP

#include <QObject>

//-------------------------------------------------------------------------------------------------------
class CommandButtons : public QObject
//-------------------------------------------------------------------------------------------------------
{
    Q_OBJECT

    public:
    enum Cmd
    {
        Play                    = 85,
        Record                  = 86,
        New                     = 87,
        Duplicate               = 88,
        Automation              = 89,
        FixedLength             = 90,
        Quantize                = 116,
        Double                  = 117,
        Delete                  = 118,
        Undo                    = 119,
        Metronome               = 9,
        TapTempo                = 3,
        LeftArrow               = 44,
        RightArrow              = 45,
        UpArrow                 = 46,
        DownArrow               = 47,
        Select                  = 48,
        Shift                   = 49,
        Note                    = 50,
        Session                 = 51,
        AddEffect               = 52,
        AddTrack                = 53,
        OctaveDown              = 54,
        OctaveUp                = 55,
        Repeat                  = 56,
        Accent                  = 57,
        Scales                  = 58,
        User                    = 59,
        Mute                    = 60,
        Solo                    = 61,
        Next                    = 62,
        Previous                = 63,
        Device                  = 110,
        Browse                  = 111,
        Track                   = 112,
        Clip                    = 113,
        Volume                  = 114,
        PanSend                 = 115,
        Fourth                  = 36,
        FourthT                 = 37,
        Eighth                  = 38,
        EighthT                 = 39,
        Sixteenth               = 40,
        SixteenthT              = 41,
        ThirtySecond            = 42,
        ThirtySecondT           = 43,
        Master                  = 28,
        Stop                    = 29
    };

    Q_ENUM ( Cmd )
};

//-------------------------------------------------------------------------------------------------------
class ToggleLightingMode : public QObject
//-------------------------------------------------------------------------------------------------------
{
    Q_OBJECT

    public:
    enum Tlm
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

    Q_ENUM ( Tlm )

};

//-------------------------------------------------------------------------------------------------------
class ButtonLightingMode : public QObject
//-------------------------------------------------------------------------------------------------------
{
    Q_OBJECT
    public:

    enum Blm
    {
        OFF = 0,
        DIM = 1,
        DIM_BLINK_SLOW = 2,
        DIM_BLINK_FAST = 3,
        FULL = 4,
        FULL_BLINK_SLOW = 5,
        FULL_BLINK_FAST = 6
    };

    Q_ENUM ( Blm )
};

//-------------------------------------------------------------------------------------------------------
class ToggleContext : public QObject
//-------------------------------------------------------------------------------------------------------
{
    Q_OBJECT
    public:

    enum Tc
    {
        TRACK_ENABLE  = 0,
        TRACK_MUTE    = 1,
        TRACK_SOLO    = 2
    };

    Q_ENUM ( Tc )
};

//-------------------------------------------------------------------------------------------------------
class PadLightingMode : public QObject
//-------------------------------------------------------------------------------------------------------
{
    Q_OBJECT
    public:

    enum Plm
    {
        OFF            = 0,
        FADE_IN_24     = 1,
        FADE_IN_16     = 2,
        FADE_IN_8      = 3,
        FADE_IN_4      = 4,
        FADE_IN_2      = 5,
        PULSE_24       = 6,
        PULSE_16       = 7,
        PULSE_8        = 8,
        PULSE_4        = 9,
        PULSE_2        = 10,
        BLINK_24       = 11,
        BLINK_16       = 12,
        BLINK_8        = 13,
        BLINK_4        = 14,
        BLINK_2        = 15
    };

    Q_ENUM ( Plm )
};

//-------------------------------------------------------------------------------------------------------
class PadColor : public QObject
//-------------------------------------------------------------------------------------------------------
{
    Q_OBJECT

    public:
    enum PadC
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

    Q_ENUM ( PadC )

};

//-------------------------------------------------------------------------------------------------------
class ToggleRow : public QObject
//-------------------------------------------------------------------------------------------------------
{
    Q_OBJECT
    public:

    enum Tr
    {
        UPPER   = 20,
        LOWER   = 102
    };

    Q_ENUM ( Tr )

};



#endif // DEVICE_ENUMS_HPP

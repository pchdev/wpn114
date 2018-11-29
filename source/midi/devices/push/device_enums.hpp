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
        Off                 = 0,
        RedDim              = 1,
        RedDimSlow          = 2,
        RedDimFast          = 3,
        RedFull             = 4,
        RedFullSlow         = 5,
        RedFullFast         = 6,
        OrangeDim           = 7,
        OrangeDimSlow       = 8,
        OrangeDimFast       = 9,
        OrangeFull          = 10,
        OrangeFullSlow      = 11,
        OrangeFullFast      = 12,
        YellowDim           = 13,
        YellowDimSlow       = 14,
        YellowDimFast       = 15,
        YellowFull          = 16,
        YellowFullSlow      = 17,
        YellowFullFast      = 18,
        GreenDim            = 19,
        GreenDimSlow        = 20,
        GreenDimFast        = 21,
        GreenFull           = 22,
        GreenFullSlow       = 23,
        GreenFullFast       = 24
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
        Off = 0,
        Dim = 1,
        DimBlinkSlow = 2,
        DimBlinkFast = 3,
        Full = 4,
        FullBlinkSlow = 5,
        FullBlinkFast = 6
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
        TrackEnable  = 0,
        TrackMute    = 1,
        TrackSolo    = 2
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
        Off          = 0,
        Fadein24     = 1,
        Fadein16     = 2,
        Fadein8      = 3,
        Fadein4      = 4,
        Fadein2      = 5,
        Pulse24      = 6,
        Pulse16      = 7,
        Pulse8       = 8,
        Pulse4       = 9,
        Pulse2       = 10,
        Blink24      = 11,
        Blink16      = 12,
        Blink8       = 13,
        Blink4       = 14,
        Blin2        = 15
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
        Black                   = 0,
        LightestWhite           = 1,
        LighterWhite            = 2,
        White                   = 3,
        Salmon                  = 4,
        Red                     = 5,
        DarkerRed               = 6,
        DarkestRed              = 7,
        Beige                   = 8,
        Orange                  = 9,
        DarkerOrange            = 10,
        DarkestOrange           = 11,
        Gold                    = 12,
        Yellow                  = 13,
        DarkerYellow            = 14,
        DarkestYellow           = 15,
        AppleGreen              = 16,
        GrassGreen              = 17,
        DarkerGrassGreen        = 18,
        DarkestGrassGreen       = 19,
        LightBlueGreen          = 20,
        Green                   = 21,
        DarkerGreen             = 22,
        DarkestGreen            = 23,
        SeaGreen                = 24,
        BlueGreen               = 25,
        GlacierGreen            = 26,
        DarkBlueGreen           = 27,
        BlueSteel               = 28,
        BlueGreen2              = 29,
        DarkBlueGreen2          = 30,
        DarkestBlueGreen        = 31,
        BlueSteel2              = 32,
        FluoBlue                = 33,
        DarkBlueGreen3          = 34,
        DarkestBlueGreen2       = 35,
        LightSkyBlue            = 36,
        SkyBlue                 = 37,
        DarkSkyBlue             = 38,
        NightSkyBlue            = 39,
        SunsetBlue              = 40,
        SkyBlue2                = 41,
        DarkSkyBlue2            = 42,
        NightSkyBlue2           = 43,
        LightPurple             = 44,
        PurplishBlue            = 45,
        DarkPurplishBlue        = 46,
        DarkestPurplishBlue     = 47,
        LightPurple2            = 48,
        Purple                  = 49,
        DarkPurple              = 50,
        DarkestPurple           = 51,
        LightFuscia             = 52,
        Fuscia                  = 53,
        DarkFuscia              = 54,
        DarkestFuscia           = 55,
        LipsPink                = 56,
        Pink                    = 57,
        DarkPink                = 58,
        DarkestPink             = 59,
        OrangeCandy             = 60,
        OrangeMandarine         = 61,
        OrangeMango             = 62,
        YellowGreen             = 63,
        DarkGreen               = 64
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
        Upper   = 20,
        Lower   = 102
    };

    Q_ENUM ( Tr )

};



#endif // DEVICE_ENUMS_HPP

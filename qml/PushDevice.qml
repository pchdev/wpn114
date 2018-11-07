import QtQuick 2.0
import WPN114 1.0 as WPN114
import WPNPush 1.0 as Push

Item
{
    id: root

    signal play         ( );
    signal record       ( );
    signal octaveDown   ( );
    signal octaveUp     ( );
    signal session      ( );
    signal track        ( );
    signal padOn        ( int idx, int velocity);
    signal padOff       ( int idx, int velocity);
    signal leftArrow    ( );
    signal rightArrow   ( );
    signal select       ( bool value );
    signal shift        ( bool value );
    signal knob         ( int idx, int value );
    signal upToggle     ( int idx, int value );
    signal downToggle   ( int idx, int value );
    signal bend         ( int value );
    signal aftertouch   ( int idx, int value );
    signal pressure     ( int value );

    function light_button ( index, mode )
    {
        midi_hdl.control(0, index, mode);
    }

    function light_pad ( index, color, mode )
    {
        midi_hdl.noteOn(mode, index+36, color );
    }

    function light_toggle ( row, idx, mode )
    {
        midi_hdl.control(0, row+idx, mode );
    }

    function lcd_display ( row, column, str )
    {
        var sysx = [ 240, 71, 127, 21, 24+row, 0, str.length+1, column, str, 247 ];
        midi_hdl.sendVariant(sysx);
    }

    function lcd_clearline ( idx )
    {
        var sysx = [ 240, 71, 127, 21, 28+idx, 0, 0, 247 ];
        midi_hdl.sendVariant(sysx);
    }

    function lcd_clear ( )
    {
        for ( var i = 0; i < 4; ++ i ) lcd_clearline(i);
    }

    function padgrid_clear ( )
    {
        for ( var i = 36; i < 100; ++i )
            midi_hdl.noteOn(0, i, 0);
    }

    function set_pitch_bend ( )
    {
        var sysx = [ 240, 71, 127, 21, 99, 0, 1, 5, 247 ];
        midi_hdl.sendVariant(sysx);
    }

    function set_modwheel ( )
    {
        var sysx = [ 240, 71, 127, 21, 99, 0, 1, 9, 247 ];
        midi_hdl.sendVariant(sysx);
    }

    WPN114.MIDIHandler //----------------------------------------------------------------- PUSH_DEVICE
    {
        id:         midi_hdl

        inDevice:   "Ableton Push User Port"
        outDevice:  "Ableton Push User Port"

        Component.onCompleted:
        {
            console.log("MIDI In Devices:", midi_hdl.inDevices() )
            console.log("MIDI Out Devices:", midi_hdl.outDevices() )
        }

        onControlReceived:
        {
            if       ( index >= 71 && index <= 79 ) root.knob( index-71, value );
            else if  ( index >= 20 && index <= 27 ) root.upToggle( index-20, value );
            else if  ( index >= 102 && index <= 109 ) root.downToggle( index-102, value );

            else if ( index === Push.CommandButtons.Select )
                root.select( value );

            else if ( index === Push.CommandButtons.Shift )
                root.shift( value );

            else if ( index === Push.CommandButtons.LeftArrow && value ) root.leftArrow();
            else if ( index === Push.CommandButtons.RightArrow && value ) root.rightArrow();
            else if ( index === Push.CommandButtons.OctaveDown && value ) root.octaveDown();
            else if ( index === Push.CommandButtons.OctaveUp && value) root.octaveUp();
            else if ( index === Push.CommandButtons.Session && value ) root.session();
            else if ( index === Push.CommandButtons.Note && value ) root.track();
        }

        onNoteOnReceived:
        {
            if ( index > 35 && index < 100 )
                root.padOn( index-36, velocity );
        }

        onNoteOffReceived:
        {
            if ( index > 35 && index < 100 )
                root.padOff( index-36, velocity );
        }

        onPitchBendReceived:            root.bend( value );
        onAftertouchReceived:           root.aftertouch( index-36, value );
        onChannelPressureReceived:      root.pressure( value );
    }
}

import QtQuick 2.0
import WPN114 1.0 as WPN114

/* A PINK NOISE BURSTS AUDIO TESTER */

Item
{
    id: root
    property int nchannels
    property WPN114.AudioStream stream
    property int index: 0
    property real duration: 2
    property string path

    function start()
    {
        loop.start();
    }

    function stop()
    {
        loop.end();
    }

    WPN114.Loop
    {
        id: loop
        source: stream
        duration: WPN114.TimeNode.Infinite
        exposePath: path
        pattern.duration: sec( root.duration )

        property int channel: 0

        onStart: { root.index = 0; channel = 0 }

        onLoop:
        {
            console.log("loop", index, channel, ashes.parentChannels )

            if ( root.index % 2 )
            {
                ashes.active = false;
                channel++;

                if ( channel === root.nchannels )
                     channel = 0;

                ashes.parentChannels = channel;
            }

            else ashes.active = true;
            root.index++;
        }

        onEnd: { ashes.active = false; ashes.parentChannels = 0 }
    }

    WPN114.Ashes
    {
        id: ashes
        parentStream: stream
        active: false
    }
}



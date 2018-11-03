import QtQuick 2.0

Item
{
    id: root
    property real rms:  96.0
    property real peak: 96.0

    onRmsChanged: canvas.requestPaint();

    Rectangle
    {
        height: parent.height - parent.height*0.1
        width:  parent.width
        y: parent.height*0.1

        border.color: "black"
        border.width: 1;

        Canvas
        {
            id: canvas
            height: parent.height-2
            width: parent.width-2;
            x: 1
            y: 1

            onPaint:
            {
                var ctx = getContext("2d");
                ctx.reset();

                // peak first
                ctx.beginPath();

                var peak0 = root.peak/48;

                if ( root.peak >= 0 )
                {
                    ctx.fillStyle = Qt.rgba(0.4, 0, 0, 1)
                    cliprect.color = "darkred";
                }

                if ( root.peak >= -6 && root.peak < 0 )
                     ctx.fillStyle = Qt.rgba(0.6, 0.6, 0, 1)
                else ctx.fillStyle = Qt.rgba(0, 0.6, 0, 1)

                var peaky = Math.abs(peak0)*height;
                var peakh = height-peaky;

                ctx.fillRect(0, peaky, width, peakh);
                ctx.closePath();

                // rms then
                ctx.beginPath();
                ctx.fillStyle = Qt.rgba(0, 0.4, 0, 1)

                var rms0 = root.rms/48;
                var y = Math.abs(rms0)*height
                var h = height-y;

                ctx.fillRect(0, y, width, h);
                ctx.closePath();
            }
        }
    }

    Rectangle
    {
        id: cliprect
        width:  parent.width
        height: parent.height*0.1
        color: "red"
        border.color: "black"
        border.width: 1;

        MouseArea
        {
            anchors.fill: parent
            onPressed: cliprect.color = "transparent"
        }
    }
}

import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import WPN114 1.0

Rectangle {

    // for quarrè:
    // sends, vumeters, rooms, fields, oneshots
    // audio groups // or tracks
    // bursts audio tests

    color: "grey"
    visible: true
    width: 640
    height: 480

    Button {
        text: "SinOsc"
        checkable: true
        onClicked: {
            sinosc_test.active = checked
        }
    }

    Button {
        text: "note"
        y: 50
        onClicked: {
            vsttest.noteOn(0, 60, 100);
        }
    }

    Slider {
        x: 100
        onValueChanged: {
            sinosc_test.frequency = value*440+220
        }
    }

    /*RoomsSetup {
        id: rooms_setup
        numOutputs: 8
        positions: [
            0.0, 0.0,
            0.0, 0.0,
            0.0, 0.0,
            0.0, 0.0,
            0.0, 0.0,
            0.0, 0.0,
            0.0, 0.0,
            0.0, 0.0 ]
        // + gui
    }*/

    AudioOut {
        id: audio_backend
        device: "Soundflower (2ch)"
        numInputs: 0
        numOutputs: 2
        sampleRate: 44100
        blockSize: 512

        Master {
            id: master
            level: 1
            muted: false

            /*Rooms {
                // inputs are automatically defined by the children AudioObjects
                // outputs are defned by the room setup object
                id: rooms_test
                setup: rooms_setup

                SinOsc {
                    id: sinosc_test
                    frequency: 440
                    level: 0.25
                    active: false
                }
            }*/

            Fields {
                id: fields
                path: "/Users/pchd/Desktop/tester.wav"
                xfade: 65536
            }

            /*AudioPlugin {
                id: vsttest
                path: "/Library/Audio/Plug-Ins/VST/Kaivo.vst"
                Component.onCompleted: {}
            }*/
        }

        Component.onCompleted: {
            audio_backend.start();
        }
    }
}

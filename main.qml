import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import WPN114 1.0

Rectangle {

    // for quarrè:
    // sends, vsts, vumeters, rooms, fields
    // independent channel dispatching
    // audio groups (tracks)
    // bursts audio tests

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

    Slider {
        x: 100
        onValueChanged: {
            sinosc_test.frequency = value*440+220
        }
    }

    RoomsSetup {
        id: rooms_setup
    }

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

            SinOsc {
                id: sinosc_test
                frequency: 440
                level: 0.25
                active: false
            }

            AudioPlugin {
                id: vsttest
                path: "/Library/Audio/Plug-Ins/VST/Kaivo.vst"
                Component.onCompleted: {
                    active = true;
                    vsttest.showEditorWindow();
                }
            }
        }

        Component.onCompleted: {
            audio_backend.start();
        }
    }
}

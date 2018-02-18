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

    // AUDIO ---------------------------------------------------------------------------

    RoomsSetup {
        id: rooms_setup

        Speaker {
            position: [0.8, 1.0]
            influence: 1.0

        }
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

            Rooms {
                id: rooms_test
                setup: rooms_setup

                Source {
                    lposition: [0.0, 0.0]
                    rposition: [0.0, 0.0]
                    influence: 0.5
                    level: 1.0

                    Fields {
                        id: fields
                        path: "/Users/pchd/Desktop/tester.wav"
                        xfade: 65536

                        Send {
                            target: altiverb
                            level: 1.0
                        }
                    }
                }

                Source {
                    lposition: [0.25, 0.5]
                    rposition: [0.75, 0.5]
                    influence: 1.0
                    level: 1.0

                    AudioPlugin {
                        id: altiverb
                        path: "/Library/Audio/Plug-Ins/VST/Audio Ease/Altiverb 7.vst"
                        active: false
                        Component.onCompleted: {}
                    }
                }
            }
        }

        Component.onCompleted: {
            audio_backend.start();
        }
    }
}

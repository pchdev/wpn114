import QtQuick 2.0
import WPN114 1.0 as WPN114
import Ossia 1.0 as Ossia

Item {

    property string sndpath: "/Users/pchd/Projects/quarre/2/sndfiles/"

    property var fsndfiles: [
        "burn.wav",
        "alpes-source.wav",
        "ghost-noise.wav",
        "noise-earth.wav",
        "noise-leaves.wav"
    ];

    property var osndfiles: [
        "boiling1.wav",
        "boiling2.wav",
        "bird-insect.wav",
        "dinosaur.wav",
        "insects.wav",
        "orage1.wav",
        "orage2.wav",
        "orage3.wav",
        "orage4.wav",
        "paroral-gran.wav"];

    HemicycliaSpeakerSetup
    {
        id: speaker_setup
    }

    WPN114.AudioOut //------------------------------------------------------------------
    {
        id: audio_output
        device: "MOTU-HEMICYCLIA"
        numInputs: 0
        numOutputs: speaker_setup.nspeakers;
        sampleRate: 44100
        blockSize: 512

        WPN114.Master //----------------------------------------------------------------
        {
            id: audio_master

            WPN114.Rooms2D
            {
                // main spatialization handler
                id: main_rooms

                WPN114.Source2D //------------------------------------------------------
                {
                    id: aux_effects

                    // the main aux effects
                    // those are mixed together and placed
                    // in the middle, with a maximum influence
                    lposition: [ 0.25, 0.5 ];
                    rposition: [ 0.75, 0.5 ];
                    influence: 1.0
                    level: 1.0

                    WPN114.AudioPlugin
                    {
                        id: altiverb
                        path: "/Library/Audio/Plug-Ins/VST/Audio Ease/Altiverb 7.vst"
                        program: 0
                    }

                    WPN114.AudioPlugin
                    {
                        id: amplitube
                        path: "/Library/Audio/Plug-Ins/VST/Amplitube 4.vst"
                        program: 0
                    }
                }

                WPN114.Source2D //------------------------------------------------------
                {
                    id: kaivo_1_src
                    // the main instrument source

                    lposition: [ 0.25, 0.1 ]
                    rposition: [ 0.75, 0.1 ]
                    influence: 0.5
                    level: 1.0

                    WPN114.AudioPlugin
                    {
                        id: kaivo_1
                        path: "/Library/Audio/Plug-Ins/VST/Kaivo.vst"
                        program: 0
                        sends: [

                            WPN114.Send {
                                target: altiverb
                                level: 0.75
                            },

                            WPN114.Send {
                                target: amplitube
                                level: 0.5
                                active: false
                            }
                        ]
                    }
                }

                WPN114.Source2D //------------------------------------------------------
                {
                    id: kaivo_2_src
                    // kaivo audio effects

                    lposition: [ 0.25, 0.1 ]
                    rposition: [ 0.75, 0.1 ]
                    influence: 0.5
                    level: 1.0

                    WPN114.AudioPlugin
                    {
                        id: kaivo_2
                        path: "/Library/Audio/Plug-Ins/VST/Kaivo.vst"
                        program: 1
                        sends: [

                            WPN114.Send {
                                target: altiverb
                                level: 0.75
                            },

                            WPN114.Send {
                                target: amplitube
                                level: 0.5
                                active: false
                            }
                        ]
                    }
                }

                WPN114.Source2D //------------------------------------------------------
                {
                    id: absynth_src

                    lposition: [ 0.25, 0.1 ]
                    rposition: [ 0.75, 0.1 ]
                    influence: 0.5
                    level: 1.0

                    WPN114.AudioPlugin
                    {
                        id: absynth
                        path: "/Library/Audio/Plug-Ins/VST/Absynth 5 Stereo.vst"
                        program: 0
                        sends: [

                            WPN114.Send {
                                target: altiverb
                                level: 0.75
                            },

                            WPN114.Send {
                                target: amplitube
                                level: 0.5
                                active: false
                            }
                        ]
                    }
                }

                WPN114.Source2D //------------------------------------------------------
                {
                    id: falcon_src

                    lposition: [ 0.25, 0.1 ]
                    rposition: [ 0.75, 0.1 ]
                    influence: 0.5
                    level: 1.0

                    WPN114.AudioPlugin
                    {
                        id: falcon
                        path: "/Library/Audio/Plug-Ins/VST/Falcon.vst"
                        program: 0
                        sends: [

                            WPN114.Send {
                                target: altiverb
                                level: 0.75
                            },

                            WPN114.Send {
                                target: amplitube
                                level: 0.5
                                active: false
                            }
                        ]
                    }

                    Repeater
                    {
                        id: fnsdfiles_arr
                        model: fsndfiles

                        WPN114.Fields
                        {
                            path: sndpath+modelData
                            xfade: 131072
                        }
                    }

                    Repeater
                    {
                        id: osdnfiles_arr
                        model: osndfiles

                        WPN114.Oneshots
                        {
                            path: sndpath+modelData
                        }
                    }
                }
            }
        }
    }
}

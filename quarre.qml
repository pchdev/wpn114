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
        "paroral-gran.wav"
    ];

    // ------------------------------------------------------------------- SPEAKER_SETUP

    HemicycliaSpeakerSetup
    {
        id: speaker_setup
    }

    WPN114.AudioOut //----------------------------------------------------  AUDIO_OUTPUT
    {
        id: audio_output
        device: "MOTU-HEMICYCLIA"
        numInputs: 0
        numOutputs: speaker_setup.nspeakers;
        sampleRate: 44100
        blockSize: 512

        Ossia.Property on active {
            node: "/backend/active"
        }

        WPN114.Master //---------------------------------------------------  AUDIO_MASTER
        {
            id: audio_master
            level: 1.0

            Ossia.Property on level {
                node: "/master/level"
            }

            Ossia.Property on muted {
                node: "/master/muted"
            }

            WPN114.Rooms2D // ---------------------------------------------  ROOMS_SPATIALIZER
            {
                id: main_rooms
                setup: speaker_setup

                WPN114.Source2D //-----------------------------------------  AUX_EFFECTS
                {
                    id: aux_effects

                    // the main aux effects
                    // those are mixed together and placed
                    // in the middle, with a maximum influence
                    positions: [ 0.25, 0.5, 0.75, 0.5 ]
                    influence: 1.0
                    level: 1.0

                    WPN114.AudioPlugin // ---------------------------------  ALTIVERB
                    {
                        id: altiverb
                        path: "/Library/Audio/Plug-Ins/VST/Audio Ease/Altiverb 7.vst"
                        program: 0

                        Ossia.Property on level { node: "/aux/altiverb/level" }
                        Ossia.Property on program { node: "/aux/altiverb/program" }
                    }

                    WPN114.AudioPlugin // --------------------------------- AMPLITUBE
                    {
                        id: amplitube
                        path: "/Library/Audio/Plug-Ins/VST/Amplitube 4.vst"
                        program: 0

                        Ossia.Property on level { node: "/aux/amplitube/level" }
                        Ossia.Property on program { node: "/aux/amplitube/program" }
                        Ossia.Property on active { node: "/aux/amplitube/active" }
                    }
                }

                WPN114.Source2D //------------------------------------------  KAIVO_INSTR
                {
                    id: kaivo_1_src
                    // the main instrument source

                    lposition: [ 0.25, 0.1 ]
                    rposition: [ 0.75, 0.1 ]
                    influence: 0.5
                    level: 1.0

                    Ossia.Property on lposition { node: "/synths/kaivo_1/position/left-channel" }
                    Ossia.Property on rposition { node: "/synths/kaivo_1/position/right-channel" }
                    Ossia.Property on influence { node: "/synths/kaivo_1/influence" }

                    WPN114.AudioPlugin // ---------------------------------
                    {
                        id: kaivo_1
                        path: "/Library/Audio/Plug-Ins/VST/Kaivo.vst"
                        program: 0

                        Ossia.Property on level { node: "/synths/kaivo_1/level" }
                        Ossia.Property on program { node: "/synths/kaivo_1/program" }

                        sends: [

                            WPN114.Send
                            {
                                target: altiverb
                                level: 0.75

                                Ossia.Property on active { node: "/synths/kaivo_1/sends/altiverb/active" }
                                Ossia.Property on level { node: "/synths/kaivo_1/sends/altiverb/level" }
                            },

                            WPN114.Send
                            {
                                target: amplitube
                                level: 0.5
                                active: false

                                Ossia.Property on active { node: "/synths/kaivo_1/sends/amplitube/active" }
                                Ossia.Property on level { node: "/synths/kaivo_1/sends/amplitube/level" }
                            }
                        ]
                    }
                }

                WPN114.Source2D //----------------------------------------------- KAIVO_EFFECTS
                {
                    id: kaivo_2_src
                    // kaivo audio effects

                    lposition: [ 0.25, 0.1 ]
                    rposition: [ 0.75, 0.1 ]
                    influence: 0.5
                    level: 1.0

                    Ossia.Property on lposition { node: "/synths/kaivo_2/position/left-channel" }
                    Ossia.Property on rposition { node: "/synths/kaivo_2/position/right-channel" }
                    Ossia.Property on influence { node: "/synths/kaivo_2/influence" }
                    Ossia.Property on level { node: "/synths/kaivo_2/level" }

                    WPN114.AudioPlugin
                    {
                        id: kaivo_2
                        path: "/Library/Audio/Plug-Ins/VST/Kaivo.vst"
                        program: 1

                        Ossia.Property on level { node: "/synths/kaivo_2/level" }
                        Ossia.Property on program { node: "/synths/kaivo_2/program" }

                        sends: [

                            WPN114.Send {
                                target: altiverb
                                level: 0.75

                                Ossia.Property on active { node: "/synths/kaivo_2/sends/altiverb/active" }
                                Ossia.Property on level { node: "/synths/kaivo_2/sends/altiverb/level" }
                            },

                            WPN114.Send {
                                target: amplitube
                                level: 0.5
                                active: false

                                Ossia.Property on active { node: "/synths/kaivo_2/sends/amplitube/active" }
                                Ossia.Property on level { node: "/synths/kaivo_2/sends/amplitube/level" }
                            }
                        ]
                    }
                }

                WPN114.Source2D //-----------------------------------------------   ABSYNTH
                {
                    id: absynth_src

                    lposition: [ 0.25, 0.1 ]
                    rposition: [ 0.75, 0.1 ]
                    influence: 0.5
                    level: 1.0

                    Ossia.Property on lposition { node: "/synths/absynth/position/left-channel" }
                    Ossia.Property on rposition { node: "/synths/absynth/position/right-channel" }
                    Ossia.Property on influence { node: "/synths/absynth/influence" }

                    WPN114.AudioPlugin
                    {
                        id: absynth
                        path: "/Library/Audio/Plug-Ins/VST/Absynth 5 Stereo.vst"
                        program: 0

                        Ossia.Property on level { node: "/synths/absynth/level" }
                        Ossia.Property on program { node: "/synths/absynth/program" }

                        sends: [

                            WPN114.Send {
                                target: altiverb
                                level: 0.75

                                Ossia.Property on active { node: "/synths/absynth/sends/altiverb/active" }
                                Ossia.Property on level { node: "/synths/absynth/sends/altiverb/level" }
                            },

                            WPN114.Send {
                                target: amplitube
                                level: 0.5
                                active: false

                                Ossia.Property on active { node: "/synths/absynth/sends/amplitube/active" }
                                Ossia.Property on level { node: "/synths/absynth/sends/amplitube/level" }
                            }
                        ]
                    }
                }

                WPN114.Source2D //-------------------------------------------------   FALCON
                {
                    id: falcon_src

                    lposition: [ 0.25, 0.1 ]
                    rposition: [ 0.75, 0.1 ]
                    influence: 0.5
                    level: 1.0

                    Ossia.Property on lposition { node: "/synths/falcon/position/left-channel" }
                    Ossia.Property on rposition { node: "/synths/falcon/position/right-channel" }
                    Ossia.Property on influence { node: "/synths/falcon/influence" }

                    WPN114.AudioPlugin
                    {
                        id: falcon
                        path: "/Library/Audio/Plug-Ins/VST/Falcon.vst"                        

                        Ossia.Property on level { node: "/synths/falcon/level" }
                        Ossia.Property on program { node: "/synths/falcon/program" }

                        Repeater
                        {
                            model: falcon.parameters

                            Ossia.Parameter
                            {
                                node: "/synths/falcon/parameters/" + modelData
                                valueType: Ossia.Type.Float
                                min: 0; max: 1;
                                bounding: Ossia.Bounding.Clip;
                            }
                        }

                        Repeater
                        {
                            model: falcon.programs

                            Ossia.Parameter
                            {
                                node: "/synths/falcon/programs/" + modelData
                                valueType: Ossia.Type.Impulse
                            }
                        }

                        sends: [

                            WPN114.Send
                            {
                                target: altiverb
                                level: 0.75

                                Ossia.Property on active { node: "/synths/falcon/sends/altiverb/active" }
                                Ossia.Property on level { node: "/synths/falcon/sends/altiverb/level" }
                            },

                            WPN114.Send
                            {
                                target: amplitube
                                level: 0.5
                                active: false

                                Ossia.Property on active { node: "/synths/falcon/sends/amplitube/active" }
                                Ossia.Property on level { node: "/synths/falcon/sends/amplitube/level" }
                            }
                        ]
                    }

                    Repeater // ---------------------------------------------------  FIELDS
                    {
                        id: fnsdfiles_arr
                        model: fsndfiles

                        WPN114.Source2D
                        {
                            WPN114.Fields
                            {
                                path: sndpath+modelData
                                xfade: 131072

                                Ossia.Property on active { node: "/fields/" + modelData + "/active" }
                                Ossia.Property on level { node: "/fields/" + modelData + "/level" }

                                sends: [

                                    WPN114.Send
                                    {
                                        target: altiverb
                                        active: false
                                        level: 1.0

                                        Ossia.Property on active { node: "/fields/" + modelData + "/sends/altiverb/active" }
                                        Ossia.Property on level { node: "/fields/" + modelData + "/sends/altiverb/level" }
                                    }
                                ]
                            }
                        }
                    }

                    Repeater // --------------------------------------------------  ONESHOTS
                    {
                        id: osdnfiles_arr
                        model: osndfiles

                        WPN114.Source2D
                        {
                            WPN114.Oneshots
                            {
                                path: sndpath+modelData

                                Ossia.Property on active { node: "/oneshots/" + modelData + "/active" }
                                Ossia.Property on level { node: "/oneshots/" + modelData + "/level" }

                                sends: [

                                    WPN114.Send
                                    {
                                        target: altiverb
                                        active: false
                                        Ossia.Property on active { node: "/oneshots/" + modelData + "/sends/altiverb/active" }
                                        Ossia.Property on level { node: "/oneshots/" + modelData + "/sends/altiverb/level" }
                                    }
                                ]
                            }
                        }
                    }
                }
            }
        }
    }
}

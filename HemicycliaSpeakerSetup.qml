import QtQuick 2.0
import WPN114 1.0 as WPN114
import Ossia 1.0 as Ossia

Item {

    property int nspeakers: 16

    WPN114.RoomsSetup
    {
        id: rmssetup

        WPN114.Speaker2D //------------------------------ JBL4430 FRONT_LEFT
        {
            id: jbl4430_fl
            output: 0

            position: [ 0.1, 0.1 ]
            influence: 0.5
            level: 1.0

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

        WPN114.Speaker2D //------------------------------ JBL4430 FRONT_RIGHT
        {
            id: jbl4430_fr
            output: 1

            position: [ 0.9, 0.1 ]
            influence: 0.5
            level: 1.0

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

        WPN114.Speaker2D //------------------------------ GENELEC8030 FRONT_MIDDLE
        {
            id: gen8030_fm
            output: 2

            // middle-front genelec on the ground
            // a bit closer to the public than the 4430
            // reduced influence
            position: [ 0.5, 0.2 ]
            influence: 0.35

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

        WPN114.Speaker2D //------------------------------ JBL4412 REAR_LEFT
        {
            id: jbl4412_rl
            output: 3

            // scope is smaller than 4430
            // maybe increase influence a bit
            position: [ 0.1, 0.9 ]
            influence: 0.65
            level: 1.0

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

        WPN114.Speaker2D //------------------------------ JBL4412 REAR_RIGHT
        {
            id: jbl4412_rr
            output: 4

            position: [ 0.9, 0.9 ]
            influence: 0.65
            level: 1.0

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

        WPN114.Speaker2D //------------------------------- GENELEC8030 REAR_MIDDLE
        {
            id: gen8030_rm
            output: 5

            // the mirror image of the one on the front
            // should be under the seats probably...
            position: [ 0.5, 0.8 ]
            influence: 0.35
            level: 1.0

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

        // UPPER DYNAUDIO BM5P RING --------------------------------------------------

        WPN114.Speaker2D //------------------------------- DYNAUDIO_BM5P FRONT_LEFT (1)
        {
            id: bm5p_up_1
            output: 6

            position: [ 0.4, 0.0 ]
            influence: 0.5
            level: 1.0

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

        WPN114.Speaker2D //------------------------------- DYNAUDIO_BM5P FRONT_RIGHT (2)
        {
            id: bm5p_up_2
            output: 7

            position: [ 0.6, 0.0 ]
            influence: 0.5
            level: 1.0

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

        WPN114.Speaker2D //------------------------------- DYNAUDIO_BM5P LATERAL_FRONT_RIGHT (3)
        {
            id: bm5p_up_3
            output: 8

            position: [ 1.0, 0.15 ]
            influence: 0.5
            level: 1.0

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

        WPN114.Speaker2D //------------------------------- DYNAUDIO_BM5P LATERAL_REAR_RIGHT (4)
        {
            id: bm5p_up_4
            output: 9

            position: [ 1.0, 0.35 ]
            influence: 0.5
            level: 1.0

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

        WPN114.Speaker2D //------------------------------- DYNAUDIO_BM5P REAR_RIGHT (5)
        {
            id: bm5p_up_5
            output: 10

            position: [ 0.6, 0.5 ]
            influence: 0.5
            level: 1.0

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

        WPN114.Speaker2D //------------------------------- DYNAUDIO_BM5P REAR_LEFT (6)
        {
            id: bm5p_up_6
            output: 11

            position: [ 0.4, 0.5 ]
            influence: 0.5
            level: 1.0

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

        WPN114.Speaker2D //------------------------------- DYNAUDIO_BM5P LATERAL_REAR_LEFT (7)
        {
            id: bm5p_up_7
            output: 12

            position: [ 0.0, 0.35 ]
            influence: 0.5
            level: 1.0

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

        WPN114.Speaker2D //------------------------------- DYNAUDIO_BM5P LATERAL_FRONT_LEFT (8)
        {
            id: bm5p_up_8
            output: 13

            position: [ 0.0, 0.15 ]
            influence: 0.5
            level: 1.0

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

        WPN114.Speaker2D //------------------------------- M_AUDIO_BX5A LATERAL_LEFT
        {
            id: bx5_l_left
            output: 14

            position: [ 0.0, 0.5 ]
            influence: 0.6
            level: 1.0

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

        WPN114.Speaker2D //------------------------------- M_AUDIO_BX5A LATERAL_RIGHT
        {
            id: bx5_l_right
            output: 15

            position: [ 1.0, 0.5 ]
            influence: 0.6
            level: 1.0

            Ossia.Property on position {}
            Ossia.Property on influence {}
            Ossia.Property on level {}
        }

    }

}

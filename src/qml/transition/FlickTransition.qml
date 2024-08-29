import QtQuick

import CCStartup

TransitionGroup {
    id: root

    leave: Transition {
        SequentialAnimation {
            ParallelAnimation {
                NumberAnimation {
                    duration: 300
                    properties: "scale"
                    to: 0.6
                    easing.type: Easing.OutBack
                    easing.overshoot: 4
                }

                NumberAnimation {
                    duration: 300
                    properties: "opacity"
                    to: 0.8
                    easing.type: Easing.OutBack
                    easing.overshoot: 4
                }
            }

            NumberAnimation {
                target: leaveProp
                duration: 200
                property: "_private_rotationAngle"
                to: 90
            }
        }
    }

    enter: Transition {
        SequentialAnimation {
            NumberAnimation {
                target: enterProp
                property: "_private_rotationAngle"
                to: 0
                duration: 200
            }

            ParallelAnimation {
                NumberAnimation {
                    duration: 300
                    properties: "scale"
                    from: 0.6
                    to: 1
                    easing.type: Easing.OutQuart
                }

                NumberAnimation {
                    duration: 300
                    properties: "opacity"
                    from: 0.8
                    to: 1
                    easing.type: Easing.OutQuart
                }
            }
        }
    }

    leaveInitialProperties: InitialProperties {
        id: leaveProp
        property real _private_rotationAngle: 0

        readonly property QtObject transform: Rotation {
            origin.x: leaveTarget ? leaveTarget.width / 2 : 0
            origin.y: leaveTarget ? leaveTarget.height / 2 : 0
            axis.x: 0; axis.y: 1; axis.z: 0
            angle: leaveProp._private_rotationAngle
        }
    }

    enterInitialProperties: InitialProperties {
        id: enterProp
        property real _private_rotationAngle: -90

        readonly property real scale: 0.6
        readonly property real opacity: 0.8

        readonly property QtObject transform: Rotation {
            origin.x: enterTarget.width / 2
            origin.y: enterTarget.height / 2
            axis.x: 0; axis.y: 1; axis.z: 0
            angle: enterProp._private_rotationAngle
        }
    }
}

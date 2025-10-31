import QtQuick

import CCStartup

TransitionGroup {
    id: root

    leave: Transition {
        NumberAnimation {
            duration: 300
            properties: "scale"
            to: 0.5
            easing.type: Easing.OutBack
            easing.overshoot: 4
        }
    }

    enter: Transition {
        NumberAnimation {
            duration: 300
            properties: "scale"
            from: 0.5
            to: 1
            easing.type: Easing.OutQuart
        }
    }
}

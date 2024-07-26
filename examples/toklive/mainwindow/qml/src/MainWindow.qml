import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import org.orange.toklive
import org.orange.quickstartup

AppStartupItem {
    id: root
    property real rotationAngle: -90
    scale: 0.6

    transform: Rotation {
        id: rotation
        origin.x: root.width/2
        origin.y: root.height/2
        axis.x: 0; axis.y: 1; axis.z: 0
        angle: root.rotationAngle
    }

    Component {
        MainPane {
            id: mainPane
            anchors.fill: parent
            anchors.margins: showShadow ? Style.item.marginLevel1 : 0

            LeftPane {
                id: leftControlBar
                height: parent.height
                anchors.left: parent.left
            }

            RightPane {
                id: rightPane
                width: parent.width - leftControlBar.width
                height: parent.height
                anchors.right: parent.right
            }
        }
    }

    onEnabledChanged: {
        if (enabled) {
            Window.window.Frameless.canWindowResize = true
            Window.window.Frameless.contentMargins = Style.item.marginLevel1
        }
    }

    StartupItem.displayPopulate: Transition {
        SequentialAnimation {
            NumberAnimation {
                property: "rotationAngle"
                to: 0
                duration: Style.item.animationDuration / 2
            }

            ParallelAnimation {
                NumberAnimation {
                    duration: Style.item.animationDuration
                    properties: "scale"
                    from: 0.6
                    to: 1
                    easing.type: Easing.OutQuart
                }

                NumberAnimation {
                    duration: Style.item.animationDuration
                    properties: "opacity"
                    from: 0.8
                    to: 1
                    easing.type: Easing.OutQuart
                }
            }
        }
    }
}

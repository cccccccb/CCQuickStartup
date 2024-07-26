import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

import org.orange.quickstartup
import org.orange.toklive

Window {
    id: root
    visible: true
    width: 800
    height: 600
    title: ""

    color: root.visibility === Window.Windowed ? "transparent" : "white"

    PreloadItem.loadingOverlay: PreloadOverlay {
        id: overlay

        Button {
            id: skipButton
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.rightMargin: 10

            Material.background: Qt.color("#CCE04F5F")
            Material.foreground: Qt.color("#FFFFFFFF")
            Material.roundedScale: Material.ExtraLargeScale
            Material.elevation: 4

            visible: preloadCountdown.running
            text: qsTr("Skip") + " " + preloadCountdown.current

            onClicked: {
                preloadCountdown.running = false
            }

            Component.onCompleted: {
               root.Frameless.moveExclude.push(skipButton)
            }

            Component.onDestruction: {
                root.Frameless.moveExclude = Array.from(root.Frameless.moveExclude).filter(r => r !== skipButton)
            }
        }

        Component.onCompleted: {
           root.Frameless.moveUnder.push(overlay)
        }

        Component.onDestruction: {
            root.Frameless.moveUnder = Array.from(root.Frameless.moveUnder).filter(r => r !== overlay)
        }
    }

    PreloadItem.autoExitOverlay: false
    PreloadItem.overlayExitWhen: !preloadCountdown.running
    PreloadItem.overlayExited: Transition {
        SequentialAnimation {
            ParallelAnimation {
                NumberAnimation {
                    duration: Style.item.animationDuration
                    properties: "scale"
                    to: 0.6
                    easing.type: Easing.OutBack
                    easing.overshoot: 4
                }

                NumberAnimation {
                    duration: Style.item.animationDuration
                    properties: "opacity"
                    to: 0.8
                    easing.type: Easing.OutBack
                    easing.overshoot: 4
                }
            }

            NumberAnimation {
                property: "rotationAngle"
                to: 90
                duration: Style.item.animationDuration / 2
            }
        }
    }

    Countdown {
        id: preloadCountdown
        interval: 10
        running: root.visible
    }

    Frameless.enabled: true
    Frameless.canWindowResize: false;
}

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Effects

import org.orange.quickstartup
import org.orange.toklive

Window {
    id: root
    visible: true
    width: 800
    height: 600
    title: "TokLive"
    color: "transparent"

    AppPreloadItem.loadingOverlay: PreloadOverlay {
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

    AppPreloadItem.autoExitOverlay: false
    AppPreloadItem.overlayExitWhen: !preloadCountdown.running
    AppPreloadItem.transitionGroup:  FlickTransition {}

    Countdown {
        id: preloadCountdown
        interval: 1
        running: root.visible
    }

    Frameless.enabled: true
    Frameless.canWindowResize: false
}

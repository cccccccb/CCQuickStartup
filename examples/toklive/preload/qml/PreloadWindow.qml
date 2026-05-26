import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Controls.Material
import QtQuick.Effects

import CCStartup
import org.orange.toklive

AppPreloadItem {
    id: root

    loadingOverlay: PreloadOverlay {
        id: overlay

        Button {
            id: skipButton
            anchors {
                right: parent.right
                top: parent.top
                topMargin: 10
                rightMargin: 10
            }

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
                root.window.Frameless.moveExclude.push(skipButton)
            }

            Component.onDestruction: {
                root.window.Frameless.moveExclude = Array.from(root.window.Frameless.moveExclude).filter(r => r !== skipButton)
            }
        }

        Component.onCompleted: {
            root.window.Frameless.moveUnder.push(overlay)
        }

        Component.onDestruction: {
            root.window.Frameless.moveUnder = Array.from(root.window.Frameless.moveUnder).filter(r => r !== overlay)
        }
    }

    autoExitOverlay: false
    overlayExitWhen: !preloadCountdown.running
    transitionGroup:  FlickTransition {}
    initialProperties: InitialProperties {
        readonly property bool visible: true
        readonly property int width: 800
        readonly property int height: 600
        readonly property string title: "TokLive"
        readonly property color color: "transparent"
    }

    Countdown {
        id: preloadCountdown
        interval: 10
        running: Qt.binding(function() { return root.visible })
    }

   onWindowChanged: {
        if (!root.window) return
        root.window.Frameless.enabled = true
        root.window.Frameless.canWindowResize = false
    }
}

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Effects

import org.orange.toklive

Control {
    id: root

    implicitWidth: background.implicitWidth

    background: Item {
        id: backItem
        implicitWidth: 90
        clip: true

        Image {
            id: _blurItem
            anchors.fill: parent
            visible: false
            source: "qrc:/leftbackground.jpg"
            fillMode: Image.PreserveAspectCrop
        }

        MultiEffect {
            source: _blurItem
            anchors.fill: _blurItem
            blurEnabled: true
            blurMax: 64
            blur: 1.0
            brightness: -0.1
            opacity: 0.4
        }
    }

    contentItem: Item {
        WindowButtonPane {
            id: windowButtonPane

            anchors.left: parent.left
            anchors.top: parent.top
        }

        LeftNavigationBar {
            anchors.top: windowButtonPane.bottom
            anchors.topMargin: 10

            width: parent.width
            height: parent.height - windowButtonPane.height - 10

            Component.onCompleted: {
                Window.window.Frameless.moveExclude.push(windowButtonPane)
            }
        }
    }
}


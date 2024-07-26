import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.orange.toklive

PopupPane {
    id: root

    padding: 10
    implicitHeight: 40
    implicitWidth: contentItem.implicitWidth + 2 * padding

    contentItem: Item {
        id: contentRoot
        readonly property int controlSpacing: 5
        implicitWidth: closeButton.width + maxButton.width + minButton.width + menuButton.width + controlSpacing * 3

        RoundAnimateButton {
            id: closeButton

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter

            hoveredIn: root.hoveredIn
            icon.iconName: "close"
            icon.iconColor: "white"
            backgroundColor: Qt.hsla(0, 1, 0.6, 0.8)
            backgroundBorderColor: Qt.hsla(0, 1, 0.2, 0.8)

            onClicked: {
                Window.window.close()
            }
        }

        RoundAnimateButton {
            id: maxButton

            hoveredIn: root.hoveredIn
            anchors.leftMargin: contentRoot.controlSpacing
            anchors.left: closeButton.right
            anchors.verticalCenter: parent.verticalCenter

            icon.iconName: (Window.window.visibility === Window.Windowed
                            || Window.window.visibility === Window.Minimized)
                           ? "maximize" : "normalize"
            icon.iconColor: "white"

            backgroundColor: Qt.hsla(0.2, 0.7, 0.4, 0.8)
            backgroundBorderColor: Qt.hsla(0.2, 1, 0.2, 0.8)

            onClicked: {
                if (Window.window.visibility === Window.Maximized) {
                    Window.window.showNormal()
                } else {
                    Window.window.showMaximized()
                }
            }
        }

        RoundAnimateButton {
            id: minButton

            hoveredIn: root.hoveredIn
            anchors.leftMargin: contentRoot.controlSpacing
            anchors.left: maxButton.right
            anchors.verticalCenter: parent.verticalCenter

            icon.iconName: "minimize"
            icon.iconColor: "white"

            backgroundColor: Qt.hsla(0.4, 0.7, 0.4, 0.6)
            backgroundBorderColor: Qt.hsla(0.4, 1, 0.2, 0.8)

            onClicked: {
                Window.window.showMinimized()
            }
        }

        RoundAnimateButton {
            id: menuButton

            hoveredIn: root.hoveredIn
            anchors.leftMargin: contentRoot.controlSpacing
            anchors.left: minButton.right
            anchors.verticalCenter: parent.verticalCenter

            icon.iconName: "menu"
            icon.iconColor: "white"

            backgroundColor: Qt.hsla(0.6, 0.7, 0.4, 0.6)
            backgroundBorderColor: Qt.hsla(0.6, 1, 0.2, 0.8)
        }
    }
}

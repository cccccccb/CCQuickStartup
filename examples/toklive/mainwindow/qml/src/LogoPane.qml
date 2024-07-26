import QtQuick
import QtQuick.Controls
import QtQuick.Effects

import org.orange.toklive

PopupPane {
    id: root

    padding: 8
    implicitWidth: contentItem.implicitWidth + 2 * padding
    implicitHeight: 40

    contentItem: Item {
        id: rowContent
        clip: true
        implicitWidth: root.hoveredIn ? logoImage.implicitWidth + textLoader.implicitWidth + 8 : logoImage.implicitWidth

        Image {
            id: logoImage
            source: "qrc:/live.png"
            sourceSize: Qt.size(24, 24)
        }

        Loader {
            id: textLoader
            active: root.hoveredIn
            anchors.left: logoImage.right
            anchors.leftMargin: 8

            sourceComponent: Text {
                text: "逗音"
                color: Qt.color("#CCE04F5F")
                font.bold: true
                font.pointSize: 12
            }
        }
    }

    background: null

    Behavior on implicitWidth {
        NumberAnimation {
            duration: Style.item.animationDuration
            easing.type: Easing.OutCirc
        }
    }
}


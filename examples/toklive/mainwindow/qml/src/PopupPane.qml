import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects

import org.orange.toklive

Control {
    id: root

    hoverEnabled: true
    z: hoveredIn ? 5 : 0
    readonly property bool hoveredIn: enabled ? hovered && hoverTimer.running : false
    property color backgroundColor: Qt.rgba(1, 1, 1, 0.7)

    background: Loader {
        active: hoveredIn

        sourceComponent: Item {
            Rectangle {
                id: _backRect
                visible: false
                anchors.fill: parent
                color: root.backgroundColor
                radius: 8
                border.color: Qt.rgba(0, 0, 0, 0.2)
                border.width: Style.item.borderWidth
            }

            MultiEffect {
                source: _backRect
                anchors.fill: _backRect
                autoPaddingEnabled: true
                shadowEnabled: true
                shadowBlur: 0.4
                shadowColor: Qt.rgba(1, 1, 1, 0.4)
            }
        }
    }

    Timer {
        id: hoverTimer
        interval: 1500
        running: root.hovered
        repeat: false
    }
}

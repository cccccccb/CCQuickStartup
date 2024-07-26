import QtQuick
import QtQuick.Effects

import org.orange.toklive

Item {
    z: -100
    readonly property bool showShadow: !(Window.window.visibility === Window.Maximized || Window.window.visibility === Window.FullScreen)

    Rectangle {
        id: _backRect
        visible: !showShadow
        anchors.fill: parent

        color: Style.item.backgroundColor
        border.color: Style.item.borderColor
        border.width: Style.item.borderWidth
        radius: showShadow ? Style.item.maxRadius : 0
    }

    Loader {
        active: showShadow
        anchors.fill: _backRect

        sourceComponent: MultiEffect {
            source: _backRect
            autoPaddingEnabled: true
            shadowEnabled: true
            shadowColor: Qt.color("#CC000000")
            shadowBlur: 0.4
        }
    }
}

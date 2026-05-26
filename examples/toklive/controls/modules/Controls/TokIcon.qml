import QtQuick
import QtQuick.Controls

import org.orange.toklive

Item {
    id: root
    implicitWidth: icon.implicitWidth
    implicitHeight: icon.implicitHeight

    property string iconName
    property int iconWidth: -1
    property int iconHeight: -1
    property color iconColor
    property bool hovered: false
    property bool pressed: false

    readonly property url iconSource: TokLive.toTokIconUrl(iconName, iconColor, hovered, pressed)

    Image {
        id: icon

        anchors.centerIn: parent
        source: root.iconSource
        asynchronous: true
        sourceSize: Qt.size(iconWidth, iconHeight)
        width: iconWidth
        height: iconHeight

        onStatusChanged: {
            if (status === Image.Error)
                console.warn("TokIcon: failed to load icon:", root.iconSource)
        }
    }
}

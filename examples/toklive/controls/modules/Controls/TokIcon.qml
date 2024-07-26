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

    Image {
        id: icon

        anchors.centerIn: parent
        source: TokLive.toTokIconUrl(iconName, iconColor, hovered, pressed)
        sourceSize: Qt.size(iconWidth, iconHeight)
        width: iconWidth
        height: iconHeight
    }
}

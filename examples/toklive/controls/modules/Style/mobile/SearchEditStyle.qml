import QtQuick

import org.orange.toklive

QtObject {
    readonly property int iconMargin: 8 / EdgeToEdge.deviceDensity
    readonly property int spacing: 5 / EdgeToEdge.deviceDensity

    readonly property color backgroundColor: Qt.rgba(0, 0, 0, 0.08)
    readonly property color placeholderTextColor: Qt.rgba(0.33, 0.33, 0.33, 0.4)

    readonly property int radius: 8 / EdgeToEdge.deviceDensity
    readonly property int focusPadding: 3 / EdgeToEdge.deviceDensity
}

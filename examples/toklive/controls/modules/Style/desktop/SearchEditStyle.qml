import QtQuick

QtObject {
    readonly property int iconMargin: 8
    readonly property int spacing: 5

    readonly property color backgroundColor: Qt.rgba(0.8, 0.8, 0.8, 0.4)
    readonly property color edittingBackgroundColor: Qt.rgba(0.9, 0.9, 0.9, 0.4)
    readonly property color placeholderTextColor: Qt.rgba(0.33, 0.33, 0.33, 0.4)

    readonly property int radius: 8
    readonly property int focusPadding: 3
}

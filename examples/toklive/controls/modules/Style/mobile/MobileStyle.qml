import QtQuick
import org.orange.toklive

QtObject {
    readonly property color backgroundColor: "white"
    readonly property color backgroundTitleColor: "#F0F0F0"

    readonly property color borderColor: Qt.rgba(0.75, 0.75, 0.75, 1)
    readonly property int borderWidth: 1
    readonly property int focusPadding: 1

    readonly property int statusBarHeight: EdgeToEdge.statusBarHeight
    readonly property int navigationBarHeight: EdgeToEdge.navigationBarHeight

    readonly property int marginLevel1: 10 / EdgeToEdge.deviceDensity

    readonly property int maxIconSize: 64 / EdgeToEdge.deviceDensity
    readonly property int mediumIconSize: 48 / EdgeToEdge.deviceDensity
    readonly property int smallIconSize: 32 / EdgeToEdge.deviceDensity

    readonly property int animationDuration: 300

    readonly property int maxSpacing: 40 / EdgeToEdge.deviceDensity
    readonly property int mediumSpacing: 20 / EdgeToEdge.deviceDensity
    readonly property int smallSpacing: 10 / EdgeToEdge.deviceDensity

    readonly property QtObject fontSize: QtObject {
        readonly property int t1: 18
        readonly property int t2: 14
        readonly property int t3: 10
    }

    readonly property SearchEditStyle searchEdit: SearchEditStyle {}
    readonly property CustomButtonStyle customButton: CustomButtonStyle {}
}

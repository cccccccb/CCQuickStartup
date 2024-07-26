import QtQuick
import org.orange.toklive

QtObject {
    readonly property color backgroundColor: "white"
    readonly property color backgroundTitleColor: "#F0F0F0"

    readonly property color borderColor: Qt.rgba(0.75, 0.75, 0.75, 1)
    readonly property int maxBorderWidth: 2
    readonly property int borderWidth: 1
    readonly property int focusPadding: 1

    readonly property int maxRadius: 8
    readonly property int marginLevel1: 10

    readonly property int maxIconSize: 32
    readonly property int mediumIconSize: 24
    readonly property int smallIconSize: 16
    readonly property int miniIconSize: 12

    readonly property int animationDuration: 300

    readonly property int maxSpacing: 40
    readonly property int mediumSpacing: 20
    readonly property int smallSpacing: 10

    readonly property QtObject fontSize: QtObject {
        readonly property int t1: 14
        readonly property int t2: 9
        readonly property int t3: 8
    }

    readonly property SearchEditStyle searchEdit: SearchEditStyle {}
    readonly property CustomButtonStyle customButton: CustomButtonStyle {}

    property date currentTime: new Date()

    readonly property QtObject extraStyle: QtObject {
        readonly property int packButtonSize: 12
        readonly property int packHoverButtonSize: 24
        readonly property color miniButtonBackgroundColor: Qt.tint("lightsteelblue", "#10FF0000")
        readonly property color miniButtonBackgroundBorderColor: Qt.tint("lightslategrey", "#40000000")

    }
}

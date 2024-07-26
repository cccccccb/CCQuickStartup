import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import org.orange.toklive

LineEdit {
    id: control
    readonly property bool editting: control.activeFocus || (text.length !== 0)
    property alias placeholder: centerIndicatorLabel.text

    state: "NONEDIT"
    states: [
        State {
            name: "EDITTING"
            when: editting
            AnchorChanges {
                target: centerIndicator
                anchors.left: searchBackground.left
                anchors.verticalCenter: searchBackground.verticalCenter
            }
            PropertyChanges {
                target: centerIndicator
                anchors.leftMargin: Style.item.searchEdit.iconMargin
            }
            PropertyChanges {
                target: centerIndicatorLabel
                color: "transparent"
            }
        },
        State {
            name: "NONEDIT"
            when: !editting
            AnchorChanges {
                target: centerIndicator
                anchors.horizontalCenter: searchBackground.horizontalCenter
                anchors.verticalCenter: searchBackground.verticalCenter
            }
            PropertyChanges {
                target: centerIndicatorLabel
                color: Style.item.searchEdit.placeholderTextColor
            }
        }
    ]

    transitions: Transition {
        AnchorAnimation {
            duration: Style.item.animationDuration
            easing.type: Easing.OutCubic
        }
        ColorAnimation {
            duration: Style.item.animationDuration
            easing.type: Easing.OutCubic
        }
    }

    Item {
        id: searchBackground
        anchors.fill: parent

        RowLayout {
            id: centerIndicator
            spacing: Style.item.searchEdit.spacing

            // Search Icon
            TokIcon {
                id: searchIcon
                iconName: "search"
                iconWidth: Style.item.smallIconSize
                iconHeight: Style.item.smallIconSize
                iconColor: editting ? control.color : Style.item.searchEdit.placeholderTextColor
                Layout.alignment: Qt.AlignVCenter

                Behavior on iconColor {
                    ColorAnimation {
                        duration: Style.item.animationDuration
                    }
                }
            }

            Text {
                id: centerIndicatorLabel
                text: qsTr("Search")
                font: control.font
                color: Style.item.searchEdit.placeholderTextColor
                verticalAlignment: Text.AlignVCenter
            }
        }

        MouseArea {
            anchors.fill: parent
            visible: !editting
            onPressed: (mouse)=> {
                control.forceActiveFocus(Qt.MouseFocusReason)
                mouse.accepted = false
            }
        }
    }
}

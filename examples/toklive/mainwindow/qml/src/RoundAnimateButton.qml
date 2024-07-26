import QtQuick
import QtQuick.Controls
import QtQuick.Effects

import org.orange.toklive

Control {
    id: root

    property color backgroundColor: Style.item.extraStyle.miniButtonBackgroundColor
    property alias backgroundBorderColor: _backRect.border.color
    property alias icon: icon
    property bool hoveredIn: false
    property alias pressedIn: mouseArea.pressed
    signal clicked()

    readonly property bool _hoveredIn: hovered || hoveredIn

    hoverEnabled: true

    state: "UNHOVERED"
    states: [
        State {
            name: "HOVERED"
            when: _hoveredIn
            PropertyChanges {
                target: root
                width: Style.item.extraStyle.packHoverButtonSize
                height: Style.item.extraStyle.packHoverButtonSize
            }

            PropertyChanges {
                target: icon
                scale: 1.0
            }
        },

        State {
            name: "UNHOVERED"
            when: !_hoveredIn
            PropertyChanges {
                target: root
                width: Style.item.extraStyle.packButtonSize
                height: Style.item.extraStyle.packButtonSize
            }

            PropertyChanges {
                target: icon
                scale: 0.0
            }
        }
    ]

    background: Item {
        Rectangle {
            id: _backRect

            anchors.fill: parent
            visible: !_hoveredIn
            color: pressedIn ? Qt.darker(root.backgroundColor, 1.2) : hovered ? Qt.lighter(root.backgroundColor, 1.2) : root.backgroundColor
            border.color: Style.item.extraStyle.miniButtonBackgroundBorderColor
            border.width: Style.item.borderWidth
            radius: _backRect.width / 2

            Behavior on color {
                ColorAnimation {
                    duration: 100
                }
            }
        }

        Loader {
            active: _hoveredIn
            anchors.fill: _backRect

            sourceComponent: MultiEffect {
                source: _backRect
                autoPaddingEnabled: true
                shadowEnabled: true
                shadowBlur: 0.4
                shadowColor: Qt.color("#CC000000")
            }
        }
    }

    contentItem: FocusScope {
        id: scope
        TokIcon {
            id: icon
            anchors.centerIn: parent
            pressed: pressedIn
            hovered: root.hovered

            iconWidth: Style.item.miniIconSize
            iconHeight: Style.item.miniIconSize
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            onClicked: {
                scope.focus = true
                root.clicked()
            }
        }
    }

    transitions: Transition {
        PropertyAnimation {
            targets: [root, icon]
            properties: "width,height,scale"
            easing.type: Easing.OutBack
        }
    }
}

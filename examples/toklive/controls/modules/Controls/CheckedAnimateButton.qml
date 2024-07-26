import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

import org.orange.toklive

CustomButton {
    id: root

    autoExclusive: true
    checkable: true
    display: Button.TextUnderIcon

    Material.background: checked ? Qt.rgba(1, 1, 1, 0.9) : "transparent"
    Material.elevation: checked ? 6 : 0
    Material.roundedScale: checked ? Material.LargeScale : Material.MediumScale

    state: checked ? "CHECKED_STATE" : "UNCHECKED_STATE"
    states: [
        State {
            name: "CHECKED_STATE"
            when: checked

            PropertyChanges {
                target: root
                icon.width: Style.item.maxIconSize
                icon.height: Style.item.maxIconSize
                font.pointSize: Style.item.fontSize.t2
                font.bold: true
            }
        },
        State {
            name: "UNCHECKED_STATE"
            when: !checked

            PropertyChanges {
                target: root
                icon.width: Style.item.mediumIconSize
                icon.height: Style.item.mediumIconSize
                font.pointSize: Style.item.fontSize.t3
                font.bold: false
            }
        }
    ]

    transitions: Transition {
        SpringAnimation {
            properties: "icon.width,icon.height"

            spring: 5; damping: 0.1;
        }

        PropertyAnimation {
            properties: "font.pointSize"

            duration: Style.item.animationDuration
            easing.type: Easing.OutBack
        }
    }
}

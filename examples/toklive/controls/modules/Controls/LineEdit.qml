import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Effects

import org.orange.toklive

TextField {
    id: root

    opacity: enabled ? 1 : 0.4
    property real iconAreaWidth: 0
    leftPadding: (root.activeFocus || text.length !== 0) ? iconAreaWidth + 2 * Style.item.searchEdit.iconMargin : 0
    selectionColor: Material.accent
    selectedTextColor: root.palette.highlightedText
    verticalAlignment: TextInput.AlignVCenter

    background: Item {
        Loader {
            anchors.fill: parent
            active: !root.activeFocus

            sourceComponent: Rectangle {
                radius: Style.item.searchEdit.radius
                color: Style.item.searchEdit.backgroundColor
            }
        }

        Loader {
            anchors {
                fill: parent
                margins: -Style.item.focusPadding
            }

            active: root.activeFocus
            sourceComponent: Item {
                Rectangle {
                    id: blurRect
                    anchors.fill: parent
                    visible: false
                    color: Style.item.searchEdit.edittingBackgroundColor
                    radius: Style.item.searchEdit.radius
                    border { width: 1; color: Qt.rgba(0.8, 0.8, 0.8, 0.7) }
                }

                MultiEffect {
                    source: blurRect
                    anchors.fill: blurRect
                    autoPaddingEnabled: true
                    shadowEnabled: true
                    shadowBlur: 0.4
                    shadowOpacity: 0.2
                    shadowColor: Qt.rgba(0, 0, 0, 1)
                }
            }
        }
    }
}

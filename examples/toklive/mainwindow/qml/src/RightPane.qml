import QtQuick
import QtQuick.Layouts

import org.orange.toklive

ColumnLayout {
    id: root

    RowLayout {
        id: titleLayout
        Item {
            Layout.fillWidth: true

            PopupSearchEdit {
                id: searchEdit
                anchors.centerIn: parent

                Component.onCompleted: {
                    Window.window.Frameless.moveExclude.push(searchEdit)
                }
            }
        }

        LogoPane {
            id: logoPane
            Layout.alignment: Qt.AlignCenter

            Component.onCompleted: {
                Window.window.Frameless.moveUnder.push(logoPane)
            }
        }

        Component.onCompleted: {
            Window.window.Frameless.moveUnder.push(titleLayout)
        }
    }

    Item {
        Layout.fillWidth: true
        Layout.fillHeight: true
    }

}

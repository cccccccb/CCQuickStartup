import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import org.orange.toklive
import CCStartup

AppStartupItem {
    id: root

    AppStartupComponent {
        MainPane {
            id: mainPane
            anchors.fill: parent
            anchors.margins: showShadow ? Style.item.marginLevel1 : 0

            LeftPopup {
                id: leftControlBar
                height: parent.height
                anchors.left: parent.left
            }

            CenterPanel {
                id: rightPane
                width: parent.width - leftControlBar.width
                height: parent.height
                anchors.right: parent.right
            }
        }
    }

    onPopulateChanged: {
        if (populate) {
            Window.window.Frameless.canWindowResize = true
            Window.window.Frameless.contentMargins = AppStartupItem.mainPane.anchors.margins
        }
    }
}

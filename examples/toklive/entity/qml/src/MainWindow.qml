import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import org.orange.toklive
import org.orange.quickstartup

AppStartupItem {
    id: root

    Component {
        MainPane {
            id: mainPane
            anchors.fill: parent
            anchors.margins: showShadow ? Style.item.marginLevel1 : 0

            LeftPane {
                id: leftControlBar
                height: parent.height
                anchors.left: parent.left
            }

            RightPane {
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
            Window.window.Frameless.contentMargins = root.AppStartupItem.mainPane.anchors.margins
        }
    }
}

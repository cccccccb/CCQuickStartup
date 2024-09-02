import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import org.orange.toklive
import CCStartup

AppStartupItem {
    id: root

    property int mainMargin
    Binding {
        when: root.loaded
        root.mainMargin: AppStartupItem.mainPane.showShadow ? Style.item.marginLevel1 : 0
    }

    AppStartupComponent {
        id: mainComp

        MainPane {
            id: mainPane
            anchors.fill: parent
            anchors.margins: root.mainMargin
        }
    }

    AppStartupComponent {
        id: leftComp
        depends: mainComp

        LeftPopup {
            id: leftControlBar
            height: parent.height - root.mainMargin * 2
            anchors.left: parent.left
            anchors.leftMargin: root.mainMargin
            anchors.top: parent.top
            anchors.topMargin: root.mainMargin
        }
    }

    AppStartupComponent {
        id: centerComp
        depends: mainComp

        CenterPanel {
            id: rightPane
            height: parent.height - root.mainMargin * 2
            anchors.right: parent.right
            anchors.rightMargin: root.mainMargin
            anchors.top: parent.top
            anchors.topMargin: root.mainMargin

            Binding {
                when: root.loaded
                rightPane.width: parent.width - AppStartupItem.leftControlBar.width - root.mainMargin
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

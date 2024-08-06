import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import org.orange.toklive

ToolBar {
    id: root

    signal homeButtonCheckedChanged
    signal favoriteButtonCheckedChanged

    Material.elevation: 0
    Material.background: "transparent"

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: Style.item.marginLevel1
        anchors.rightMargin: Style.item.marginLevel1
        anchors.topMargin: 0
        spacing: Style.item.mediumSpacing

        Item {
            Layout.alignment: Qt.AlignCenter
            Layout.maximumHeight: parent.height / 5 - Style.item.marginLevel1
            Layout.fillWidth: true
            Layout.preferredHeight: 60

            CheckedAnimateButton {
                id: home_button
                icon.name: checked ? "home_checked" : "home"
                text: qsTr("Home")
                checked: true
                anchors.horizontalCenter: parent.horizontalCenter

                onCheckedChanged: root.homeButtonCheckedChanged()
            }
        }

        Item {
            Layout.alignment: Qt.AlignCenter
            Layout.maximumHeight: parent.height / 5 - Style.item.marginLevel1
            Layout.fillWidth: true
            Layout.preferredHeight: 60

            CheckedAnimateButton {
                id: category_button
                icon.name: checked ? "category_checked" : "category"
                text: qsTr("Category")
                anchors.horizontalCenter: parent.horizontalCenter

                onCheckedChanged: root.favoriteButtonCheckedChanged()
            }
        }

        Item {
            Layout.alignment: Qt.AlignCenter
            Layout.maximumHeight: parent.height / 5 - Style.item.marginLevel1
            Layout.fillWidth: true
            Layout.preferredHeight: 60

            CheckedAnimateButton {
                id: label_button
                icon.name: checked ? "label_checked" : "label"
                text: qsTr("Label")
                anchors.horizontalCenter: parent.horizontalCenter

                onCheckedChanged: root.favoriteButtonCheckedChanged()
            }
        }

        Item {
            Layout.alignment: Qt.AlignCenter
            Layout.maximumHeight: parent.height / 5 - Style.item.marginLevel1
            Layout.fillWidth: true
            Layout.preferredHeight: 60

            CheckedAnimateButton {
                id: community_button
                icon.name: checked ? "community_checked" : "community"
                text: qsTr("Square")
                anchors.horizontalCenter: parent.horizontalCenter

                onCheckedChanged: root.favoriteButtonCheckedChanged()
            }
        }

        Item {
            Layout.alignment: Qt.AlignCenter
            Layout.maximumHeight: parent.height / 5 - Style.item.marginLevel1
            Layout.fillWidth: true
            Layout.preferredHeight: 60

            CheckedAnimateButton {
                id: favorite_button
                icon.name: checked ? "favorite_checked" : "favorite"
                text: qsTr("Favorite")
                anchors.horizontalCenter: parent.horizontalCenter

                onCheckedChanged: root.favoriteButtonCheckedChanged()
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        ButtonGroup {
            buttons: [home_button, category_button, label_button, community_button, favorite_button]
        }
    }
}


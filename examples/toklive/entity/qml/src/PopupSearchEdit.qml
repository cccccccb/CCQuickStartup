import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

import org.orange.toklive

PopupPane {
    id: root

    hoverEnabled: !contentItem.editting
    implicitWidth: contentItem.implicitWidth + 2 * padding
    implicitHeight: contentItem.implicitHeight + 2 * padding

    contentItem: SearchEdit {
        implicitWidth: (editting || !root.hoveredIn) ? 200 : 250
    }

    Behavior on implicitWidth {
        NumberAnimation {
            easing.type: Easing.OutBack
        }
    }
}

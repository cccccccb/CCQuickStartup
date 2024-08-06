import QtQuick

Item {
    id: root
    property int interval: 5
    property int current: interval
    property bool running: false

    Timer {
        id: innerTimer
        running: root.running
        repeat: true
        interval: 1000
        onTriggered: {
            root.current--
            if (root.current === 0) {
                root.running = false
                root.current = interval
            }
        }
    }

}

pragma Singleton

import QtQuick
import org.orange.toklive

Loader {
    source: TokLive.isAndroid ? "mobile/MobileStyle.qml" : "desktop/DesktopStyle.qml"
}

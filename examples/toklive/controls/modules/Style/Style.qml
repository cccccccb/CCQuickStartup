pragma Singleton

import QtQuick
import org.orange.toklive.impl

Loader {
    source: TokLive.isAndroid ? "mobile/MobileStyle.qml" : "desktop/DesktopStyle.qml"
}

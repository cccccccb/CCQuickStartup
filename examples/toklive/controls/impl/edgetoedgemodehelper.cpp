#include "edgetoedgemodehelper.h"

#ifdef Q_OS_ANDROID
#include <QJniObject>
#endif

void EdgeToEdgeModeHelper::enable()
{
    enableEdgeToEdge();
}

int EdgeToEdgeModeHelper::getStatusBarHeight()
{
    int height = 0;

#ifdef Q_OS_ANDROID
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    QJniObject resources = activity.callObjectMethod("getResources", "()Landroid/content/res/Resources;");
    auto status_bar_height_object = QJniObject::fromString("status_bar_height");
    auto dimen_object = QJniObject::fromString("dimen");
    auto android_object = QJniObject::fromString("android");

    jint resourceId = resources.callMethod<jint>("getIdentifier", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I",
                                                 status_bar_height_object.object<jstring>(),
                                                 dimen_object.object<jstring>(),
                                                 android_object.object<jstring>());
    if (resourceId) {
        height = resources.callMethod<jint>("getDimensionPixelSize", "(I)I", resourceId);
    }
#endif

    return height;
}

int EdgeToEdgeModeHelper::getNavigationBarHeight()
{
    int height = 0;

#ifdef Q_OS_ANDROID
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    QJniObject resources = activity.callObjectMethod("getResources", "()Landroid/content/res/Resources;");

    auto navigation_bar_height_object = QJniObject::fromString("navigation_bar_height");
    auto dimen_object = QJniObject::fromString("dimen");
    auto android_object = QJniObject::fromString("android");
    jint resourceId = resources.callMethod<jint>("getIdentifier", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I",
                                                       navigation_bar_height_object.object<jstring>(),
                                                       dimen_object.object<jstring>(),
                                                       android_object.object<jstring>());
    if (resourceId) {
        height = resources.callMethod<jint>("getDimensionPixelSize", "(I)I", resourceId);
    }
#endif

    return height;
}

float EdgeToEdgeModeHelper::getDeviceDensity()
{
    int density = 1;

#ifdef Q_OS_ANDROID
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    QJniObject resources = activity.callObjectMethod("getResources", "()Landroid/content/res/Resources;");
    QJniObject displayMetrics = resources.callObjectMethod("getDisplayMetrics", "()Landroid/util/DisplayMetrics;");

    density = displayMetrics.getField<float>("density");
#endif

    return density;
}

int EdgeToEdgeModeHelper::statusBarHeight() const
{
    return m_statusBarHeight;
}

int EdgeToEdgeModeHelper::navigationBarHeight() const
{
    return m_navigationBarHeight;
}

qreal EdgeToEdgeModeHelper::deviceDensity() const
{
    return m_deviceDensity;
}

void EdgeToEdgeModeHelper::enableEdgeToEdge()
{
#ifdef Q_OS_ANDROID
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([this]() {
        QJniObject activity = QNativeInterface::QAndroidApplication::context();
        if (!activity.isValid()) {
            qDebug() << "Invalid activity object";
            return;
        }

        QJniObject::callStaticMethod<void>(
            "org/orange/toklive/EdgeToEdgeHelper",
            "enable",
            "(Landroid/app/Activity;Z)V",
            activity.object<jobject>(),
            true);

        QJniEnvironment env;
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }

        auto statusBarHeight = getStatusBarHeight();
        auto navigationBarHeight = getNavigationBarHeight();
        auto deviceDensity = getDeviceDensity();

        if (m_deviceDensity != deviceDensity) {
            m_deviceDensity = deviceDensity;
            Q_EMIT deviceDensityChanged();
        }

        int statusBarHeight_device = static_cast<float>(statusBarHeight / m_deviceDensity);
        if (m_statusBarHeight != statusBarHeight_device) {
            m_statusBarHeight = statusBarHeight_device;
            Q_EMIT statusBarHeightChanged();
        }

        int navigationBarHeight_device = static_cast<float>(navigationBarHeight / m_deviceDensity);
        if (m_navigationBarHeight != navigationBarHeight_device) {
            m_navigationBarHeight = navigationBarHeight_device;
            Q_EMIT navigationBarHeightChanged();
        }
    });
#endif
}

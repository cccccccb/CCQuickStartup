#ifndef EDGETOEDGEMODEHELPER_H
#define EDGETOEDGEMODEHELPER_H

#include <QObject>
#include <QCoreApplication>
#include <QFutureWatcher>
#include <QtQml>

class EdgeToEdgeModeHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int statusBarHeight READ statusBarHeight NOTIFY statusBarHeightChanged FINAL)
    Q_PROPERTY(int navigationBarHeight READ navigationBarHeight NOTIFY navigationBarHeightChanged FINAL)
    Q_PROPERTY(qreal deviceDensity READ deviceDensity NOTIFY deviceDensityChanged FINAL)

    QML_SINGLETON
    QML_NAMED_ELEMENT(EdgeToEdge)

public:
    explicit EdgeToEdgeModeHelper(QObject *parent = nullptr);

    Q_INVOKABLE void enable();

    int statusBarHeight() const;
    int navigationBarHeight() const;
    qreal deviceDensity() const;

signals:
    void statusBarHeightChanged();
    void navigationBarHeightChanged();
    void deviceDensityChanged();

private:
    void enableEdgeToEdge();
    int fetchStatusBarHeight();
    int fetchNavigationBarHeight();
    float fetchDeviceDensity();

private:
    Q_DISABLE_COPY_MOVE(EdgeToEdgeModeHelper)
    QFutureWatcher<QVariant> *m_watcher = nullptr;
    int m_statusBarHeight = 0;
    int m_navigationBarHeight = 0;
    qreal m_deviceDensity = 1;
};

#endif // EDGETOEDGEMODEHELPER_H

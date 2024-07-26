#ifndef EDGETOEDGEMODEHELPER_H
#define EDGETOEDGEMODEHELPER_H

#include <QObject>
#include <QCoreApplication>
#include <QFutureWatcher>

class EdgeToEdgeModeHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int statusBarHeight READ statusBarHeight NOTIFY statusBarHeightChanged FINAL)
    Q_PROPERTY(int navigationBarHeight READ navigationBarHeight NOTIFY navigationBarHeightChanged FINAL)
    Q_PROPERTY(qreal deviceDensity READ deviceDensity NOTIFY deviceDensityChanged FINAL)

public:
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
    int getStatusBarHeight();
    int getNavigationBarHeight();
    float getDeviceDensity();

private:
    QFutureWatcher<QVariant> *m_watcher = nullptr;
    int m_statusBarHeight = 0;
    int m_navigationBarHeight = 0;
    qreal m_deviceDensity = 1;
};

#endif // EDGETOEDGEMODEHELPER_H

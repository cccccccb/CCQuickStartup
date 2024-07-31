// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DQUICKAppStartupITEM_H
#define DQUICKAppStartupITEM_H

#include <QQuickItem>
#include <QQuickWindow>
#include "ccquickstartup_global.h"

QT_BEGIN_NAMESPACE
class QQmlComponent;
class QQuickWindow;
QT_END_NAMESPACE

class QuickAppStartupItemPrivate;
class CC_QUICKSTARTUP_EXPORT AppStartupItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QQuickWindow *containWindow READ containWindow NOTIFY containWindowChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool asynchronous READ asynchronous WRITE setAsynchronous NOTIFY asynchronousChanged)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QML_NAMED_ELEMENT(AppStartupItem)
#endif

public:
    explicit AppStartupItem(QQuickItem *parentItem = nullptr);

    bool loaded() const;
    void setLoaded(bool loaded);

    qreal progress() const;
    void setProgress(qreal progress);

    QQuickWindow *containWindow() const;
    void setContainWindow(QQuickWindow *w);

    bool asynchronous() const;
    void setAsynchronous(bool a);

Q_SIGNALS:
    void loadedChanged();
    void progressChanged();
    void containWindowChanged();
    void asynchronousChanged();

private:
    QList<QQmlComponent *> _childrenComponents;
    QQuickWindow *_window = nullptr;
    bool _loaded = false;
    bool _asynchronous = false;
    qreal _progress = 0.0;
};

#endif  // DQUICKAppStartupITEM_H

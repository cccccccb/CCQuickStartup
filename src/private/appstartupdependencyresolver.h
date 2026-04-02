#ifndef APPSTARTUPDEPENDENCYRESOLVER_H
#define APPSTARTUPDEPENDENCYRESOLVER_H

#include <QObject>
#include <QList>
#include <QSet>
#include <QHash>
#include <QString>

class AppStartupComponent;

class AppStartupDependencyResolver
{
public:
    struct ResolutionResult {
        bool success;

        QList<AppStartupComponent*> sortedComponents;
        QList<QList<AppStartupComponent*>> batches;

        QString errorString;
        QList<AppStartupComponent*> circularChain;
    };

    static ResolutionResult resolve(const QList<AppStartupComponent*>& components);

private:
    static bool findCyclePath(AppStartupComponent* current, const QHash<AppStartupComponent*, QList<AppStartupComponent*>>& adj,
                                QSet<AppStartupComponent*>& visited,  QSet<AppStartupComponent*>& recursionStack, QList<AppStartupComponent*>& resultPath);
};

#endif // APPSTARTUPDEPENDENCYRESOLVER_H

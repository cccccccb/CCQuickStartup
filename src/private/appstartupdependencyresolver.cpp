#include "appstartupdependencyresolver.h"
#include "items/appstartupcomponent.h"

#include <QQmlListProperty>
#include <QDebug>

static QList<AppStartupComponent*> qmlListToQList(QQmlListProperty<AppStartupComponent> prop)
{
    QList<AppStartupComponent*> list;
    qsizetype count = prop.count(&prop);
    for (qsizetype i = 0; i < count; ++i) {
        AppStartupComponent* item = prop.at(&prop, i);
        if (item) {
            list.append(item);
        }
    }
    return list;
}

AppStartupDependencyResolver::ResolutionResult AppStartupDependencyResolver::resolve(const QList<AppStartupComponent *> &components)
{
    ResolutionResult result;
    result.success = true;

    if (components.isEmpty()) {
        return result;
    }

    QSet<AppStartupComponent*> componentSet(components.begin(), components.end());
    QHash<AppStartupComponent*, QList<AppStartupComponent*>> adjacencyList;

    QHash<AppStartupComponent*, int> inDegree;

    for (auto comp : components) {
        inDegree[comp] = 0;
    }

    for (auto dependent : components) {
        QList<AppStartupComponent*> dependencies = qmlListToQList(dependent->depends());

        for (auto dependency : dependencies) {
            if (dependency == dependent) {
                result.success = false;
                result.errorString = QString("Critical Error: Component '%1' depends on itself.")
                                         .arg(dependent->objectName().isEmpty() ? "Unnamed" : dependent->objectName());
                return result;
            }

            if (!componentSet.contains(dependency)) {
                result.success = false;
                result.errorString = QString("Critical Error: Component '%1' depends on an external or unmanaged component '%2'.")
                                         .arg(dependent->objectName().isEmpty() ? "Unnamed" : dependent->objectName())
                                         .arg(dependency->objectName().isEmpty() ? "Unnamed" : dependency->objectName());
                return result;
            }

            adjacencyList[dependency].append(dependent);
            inDegree[dependent]++;
        }
    }

    QList<AppStartupComponent*> queue;
    for (auto comp : components) {
        if (inDegree[comp] == 0) {
            queue.append(comp);
        }
    }

    // 开始分层处理
    while (!queue.isEmpty()) {
        result.batches.append(queue);

        QList<AppStartupComponent*> nextLayerQueue;

        for (auto u : queue) {
            result.sortedComponents.append(u);
            if (adjacencyList.contains(u)) {
                for (auto v : adjacencyList[u]) {
                    inDegree[v]--; // 既然 u 好了，v 的等待数减 1
                    if (inDegree[v] == 0) {
                        nextLayerQueue.append(v);
                    }
                }
            }
        }

        // 进入下一层
        queue = nextLayerQueue;
    }

    // 如果排序后的数量不等于原始总数，说明图中存在环（剩余的节点入度永远 > 0）
    if (result.sortedComponents.size() != components.size()) {
        result.success = false;
        result.errorString = QString("Circular dependency detected! Only %1 of %2 components could be resolved.")
                                 .arg(result.sortedComponents.size())
                                 .arg(components.size());

        // 为了给出严谨的错误报告，我们需要找出具体的环路
        // 收集所有未能解决的节点（入度 > 0）
        QList<AppStartupComponent*> remainingNodes;
        for (auto comp : components) {
            if (inDegree[comp] > 0) {
                remainingNodes.append(comp);
            }
        }

        if (!remainingNodes.isEmpty()) {
            QSet<AppStartupComponent*> visited;
            QSet<AppStartupComponent*> recursionStack;
            QList<AppStartupComponent*> path;

            // 在剩余的子图中运行 DFS 找环
            for (auto node : remainingNodes) {
                if (findCyclePath(node, adjacencyList, visited, recursionStack, path)) {
                    result.circularChain = path;
                    break; // 找到一个环就够了
                }
            }
        }
    }

    return result;
}

bool AppStartupDependencyResolver::findCyclePath(AppStartupComponent *current,
                                                 const QHash<AppStartupComponent *, QList<AppStartupComponent *>> &adj,
                                                 QSet<AppStartupComponent *> &visited,
                                                 QSet<AppStartupComponent *> &recursionStack,
                                                 QList<AppStartupComponent *> &resultPath)
{
    visited.insert(current);
    recursionStack.insert(current);
    resultPath.append(current);

    if (adj.contains(current)) {
        for (auto neighbor : adj[current]) {
            // 注意：我们只关心那些还没被“解决”的节点，即入度>0的节点。
            // 但在这个 DFS 上下文中，adj 包含所有边。
            // 只要 recursionStack 包含 neighbor，就说明找到了回边，即闭环。

            if (recursionStack.contains(neighbor)) {
                // 找到闭环！
                // 将闭环点加进去闭合路径
                resultPath.append(neighbor);
                return true;
            }

            if (!visited.contains(neighbor)) {
                if (findCyclePath(neighbor, adj, visited, recursionStack, resultPath)) {
                    return true;
                }
            }
        }
    }

    // 回溯
    recursionStack.remove(current);
    resultPath.removeLast();
    return false;
}

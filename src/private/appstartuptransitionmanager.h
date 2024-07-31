#ifndef APPSTARTUPTRANSITIONMANAGER_H
#define APPSTARTUPTRANSITIONMANAGER_H

#include <private/qquicktransitionmanager_p_p.h>

class AppStartUpTransitionManager : public QQuickTransitionManager
{
public:
    explicit AppStartUpTransitionManager(const std::function<void()> &finishedCallback)
        : QQuickTransitionManager()
        , _finishedCallback(finishedCallback)
    { }

    void setFinishedCallback(const std::function<void()> &finishedCallback) {
        _finishedCallback = finishedCallback;
    }

protected:
    void finished() override {
        if (_finishedCallback)
            _finishedCallback();
    }

private:
    std::function<void()> _finishedCallback;
};

#endif // APPSTARTUPTRANSITIONMANAGER_H

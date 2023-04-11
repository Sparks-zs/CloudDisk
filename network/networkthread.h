#ifndef NETWORKTHREAD_H
#define NETWORKTHREAD_H

#include <QThread>
#include <QWaitCondition>
#include <QReadWriteLock>
#include <QQueue>
#include "networkcontroller.h"

namespace CloudDisk{
class NetworkThread : public QThread
{
    Q_OBJECT
public:
    explicit NetworkThread(QObject *parent = nullptr);

    void stop() { _isfinish = false; }
    void addController(NetworkController::ControllerPtr controller);

protected:
    void run() override;

private:
    bool _isfinish;
    QQueue<NetworkController::ControllerPtr> _controllers;
    QReadWriteLock _rwLocker;
    QWaitCondition _waiter;

};

}

#endif // NETWORKTHREAD_H

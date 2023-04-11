#include "networkthread.h"

namespace CloudDisk{

NetworkThread::NetworkThread(QObject *parent)
    : QThread{parent},
      _isfinish(false)
{

}

void NetworkThread::addController(NetworkController::ControllerPtr controller)
{
    _rwLocker.lockForWrite();
    _controllers.push_back(controller);
    _rwLocker.unlock();
    _waiter.wakeAll();
}

void NetworkThread::run()
{
    _isfinish = true;

    while (_isfinish) {
        if(_controllers.empty()){
            _rwLocker.lockForRead();
            _waiter.wait(&_rwLocker);
            _rwLocker.unlock();
            continue;
        }
        assert(!_controllers.empty());

        NetworkController::ControllerPtr controller = _controllers.front();
        _controllers.pop_front();
    }
}

}

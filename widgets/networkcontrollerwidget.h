#ifndef NETWORKCONTROLLERWIDGET_H
#define NETWORKCONTROLLERWIDGET_H

#include <QWidget>
#include <QProgressBar>

#include "../network/networkcontroller.h"

namespace CloudDisk{

class NetworkControllerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkControllerWidget(
            NetworkController* controller,
            int id,
            QWidget *parent = nullptr);
    ~NetworkControllerWidget();

    void init();
    double computeAcceptSize(quint64 size);
    QIcon icon() { return _icon; }
    int id() { return _id; }
    void setId(int id) { _id = id; }
    double size(){ return _size; }
    QString speed() { return _speed; }
    QString unit() { return _unitFileSize; }
    QString fileName(){ return _controller->getFileInfo().name(); }
    QProgressBar* progressBar() { return _progressBar; }

signals:
    void speedChanged(QString speed);

public slots:
    void onProgress(quint64 bytes, quint64 bytesTotal);

private:
    NetworkController* _controller;
    int _id;

    QProgressBar* _progressBar;
    QIcon _icon;

    double _size;
    quint64 _preBytes;
    QString _unitFileSize, _unitBytesSize;
    QString _speed;
};

};
#endif // NETWORKCONTROLLERWIDGET_H

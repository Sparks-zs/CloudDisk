#include "networkcontrollerwidget.h"


namespace CloudDisk{

NetworkControllerWidget::NetworkControllerWidget(
        NetworkController* controller,
        int id,
        QWidget *parent) :
    QWidget(parent),
    _controller(controller),
    _id(id), _size(0), _preBytes(0),
    _unitFileSize('B'), _unitBytesSize('B'),
    _speed("0B/s")
{
    init();
}

NetworkControllerWidget::~NetworkControllerWidget()
{

}

void NetworkControllerWidget::init()
{
    switch(_controller->getFileInfo().type()){
    case FileInfo::FOLDER:
        _icon.addFile(":/images/file/folder");
        break;
    case FileInfo::TXT:
        _icon.addFile(":/images/file/txt.svg");
        break;
    case FileInfo::PNG:
        _icon.addFile(":/images/file/png.svg");
        break;
    case FileInfo::JPG:
        _icon.addFile(":/images/file/jpg.svg");
        break;
    case FileInfo::MP4:
        _icon.addFile(":/images/file/mp4.svg");
        break;
    case FileInfo::ZIP:
        _icon.addFile(":/images/file/zip.svg");
        break;
    default:
        _icon.addFile(":/images/file/unknown.svg");
        break;
    }

    size_t size = _controller->getFileInfo().size();

    int level = 0;
    while(size > 1024 * 1024){
        size = size >> 10;
        level ++;
    }

    _size = size;
    if(_size > 1024){
        _size /= 1024;
        level++;
    }

    switch(level){
    case 0:
        _unitFileSize = 'B';
        break;
    case 1:
        _unitFileSize = "KB";
        break;
    case 2:
        _unitFileSize = "MB";
        break;
    case 3:
        _unitFileSize = "GB";
        break;
    case 4:
        _unitFileSize = "TB";
        break;
    default:
        _unitFileSize = "B";
        break;
    }

    _progressBar = new QProgressBar(this);
    _progressBar->setValue(0);

    connect(_controller, &NetworkController::uploadProgress, this, &NetworkControllerWidget::onProgress);
    connect(_controller, &NetworkController::downloadProgress, this, &NetworkControllerWidget::onProgress);
}

void NetworkControllerWidget::onProgress(quint64 bytes, quint64 bytesTotal)
{

    double process = static_cast<double>(bytes) / static_cast<double>(bytesTotal);
    _progressBar->setFormat(QString::fromLocal8Bit("%1%").arg(QString::number(process * 100, 'f', 2)));
    _progressBar->setValue(process * 100);

    double size = computeAcceptSize(bytes - _preBytes);

    if(_controller->isUploading()){
        _speed = QString::number(size / _controller->getUploadElapsedTime(), 'f', 2) + _unitBytesSize + "/s";
    }
    else{
        _speed = QString::number(size / _controller->getDownloadElapsedTime(), 'f', 2) + _unitBytesSize + "/s";
    }

    _preBytes = bytes;
    emit speedChanged(_speed);
}

double NetworkControllerWidget::computeAcceptSize(quint64 size)
{
    int level = 0;
    while(size > 1024 * 1024){
        size = size >> 10;
        level ++;
    }

    double s = size;
    if(s > 1024){
        s /= 1024;
        level++;
    }

    switch(level){
    case 0:
        _unitBytesSize = 'B';
        break;
    case 1:
        _unitBytesSize = "KB";
        break;
    case 2:
        _unitBytesSize = "MB";
        break;
    case 3:
        _unitBytesSize = "GB";
        break;
    case 4:
        _unitBytesSize = "TB";
        break;
    default:
        _unitBytesSize = "B";
        break;
    }
    return s;
}

}

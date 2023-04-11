#include <QDir>
#include <QFileInfo>

#include "networkcontroller.h"
#include "../config.h"

namespace CloudDisk{

NetworkController::NetworkController(QObject *parent)
    : QObject{parent},
      _net(new HttpNetwork(this)),
      _url(URL),
      _isDownloading(false),
      _isUploading(false),
      _uploadSize(UPLOAD_SIZE),
      _downloadSize(DOWNLOAD_SIZE),
      _uploadElapsedTime(0),
      _downloadElapsedTime(0)
{
    connect(_net.get(), &HttpNetwork::readyRead, this, &NetworkController::onReadyRead);
    connect(_net.get(), &HttpNetwork::finished, this, &NetworkController::onfinished);
}

void NetworkController::upload(const QString& localFilePath, const QString& hostFilePath)
{
    if(_isUploading){
        return;
    }

    _uploadFile.setFileName(localFilePath);
    if(!_uploadFile.open(QFile::ReadOnly)) return;

    _localFilePath = localFilePath;
    _hostFilePath = hostFilePath;
    startUpload(0, _uploadSize);
}

void NetworkController::download(const QString& localFilePath, const QString& hostFilePath, qint64 hostFileSize)
{
    if(_isDownloading){
        return;
    }

    QStringList pathlist = localFilePath.split('/');
    QString filename = pathlist.back();
    pathlist.pop_back();
    QString filedir;
    for(auto& path : pathlist){
        filedir += path + '/';
    }
    QDir dir(filedir);
    if(!dir.exists()){
        dir.mkpath(filedir);
    }

    _downloadFile.setFileName(localFilePath);
    if(_downloadFile.exists()){
        _downloadFile.remove();
    }

    if(!_downloadFile.open(QFile::ReadWrite)) return;

    _localFilePath = localFilePath;
    _hostFilePath = hostFilePath;
    _hostFileSize = hostFileSize;

    startDownload(0, _downloadSize);
}

void NetworkController::startUpload(quint64 from, quint64 to)
{
    _isUploading = true;

    QByteArray data = _read(from, to);
    QString query = "path=" + _hostFilePath + '&' + \
                    "from=" + QString::number(from) + '&' + \
                     "to=" + QString::number(from + data.size());

    QNetworkRequest request(_url + "/file?" + query);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    _net->post(request, data);

    _uploadTimer.start();
}

void NetworkController::startDownload(quint64 from, quint64 to)
{
    _isDownloading = true;

    QString query = "path=" + _hostFilePath;
    QString range = "bytes=" + QString::number(from) + "-" + QString::number(to);

    QNetworkRequest request(_url + "/file?" + query);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    request.setRawHeader("Range", range.toLocal8Bit());
    _net->get(request);

    _downloadTimer.start();
}

QByteArray NetworkController::_read(quint64 from, quint64 to)
{
    QByteArray data;
    qint64 fileSize = _uploadFile.size();
    if(from >= fileSize){
        return data;
    }

    if(to > 0 && to > from){
        _uploadFile.seek(from);
        qint64 size = qMin(to - from, fileSize - from);
        data = _uploadFile.read(size);
    }

    return data;
}

quint64 NetworkController::_write(quint64 from, const QByteArray& data)
{
    qint64 size = 0;

    if(_downloadFile.isOpen()){
        if(from > _downloadFile.size()){
            return 0;
        }
        _downloadFile.seek(from);
        size = _downloadFile.write(data);
    }

    return size;
}

void NetworkController::_handleUpload(const QJsonObject& json)
{
    _uploadElapsedTime = _uploadTimer.elapsed();
    QFileInfo info(_localFilePath);
    if(json["code"] == 0){
       QJsonObject data = json["data"].toObject();
       qint64 to = data["to"].toInteger();
       emit uploadProgress(to, info.size());

       if(to >= info.size()){
           _isUploading = false;
           _uploadFile.close();
           emit uploadFinished();
           qDebug() << "上传成功";
       }
       else{
           startUpload(to, to + _uploadSize);
       }
    }
    else{
       _isUploading = false;
       qDebug() << "上传失败" << json["msg"];
    }
}

void NetworkController::_handleDownload(const QByteArray& data)
{
    _downloadElapsedTime = _downloadTimer.elapsed();
    _downloadTimer.restart();

    QString rangestr = _net->getHeader("Content-Range");
    qint64 pos = rangestr.indexOf("bytes ");
    rangestr = rangestr.right(rangestr.length() - (pos + 6));

    QStringList rangeList = rangestr.split('/');
    QStringList fromtolist = rangeList[0].split('-');
    quint64 size = rangeList[1].toULongLong();
    quint64 from = fromtolist[0].toULongLong();
    quint64 to = fromtolist[1].toULongLong();

    if(!rangeList.empty() && rangeList.size() == 2){
        emit downloadProgress(to, size);
        if(to >= size - 1){
            _write(from, data);

            _isDownloading = false;
            if(_downloadFile.isOpen())
                _downloadFile.close();

            qDebug() << "下载成功";
            emit downloadFinished();
        }
        else{
            startDownload(to, to + _downloadSize);
            _write(from, data);
        }

    }
    else{
        _isDownloading = false;
        qDebug() << "下载失败";
    }
}

void NetworkController::onfinished()
{
    if(_isUploading){
        QJsonObject obj = _byteArray2Json(_data);

        if(obj.isEmpty()){
            return;
        }
        _handleUpload(obj);
    }
    else if(_isDownloading){
        _handleDownload(_data);
    }

    _data.clear();
}

void NetworkController::onReadyRead(const QByteArray& data)
{
    _data.append(data);
}

QByteArray NetworkController::_json2ByteArray(const QJsonObject& json)
{
    QJsonDocument doc(json);
    return doc.toJson(QJsonDocument::Compact);
}

QJsonObject NetworkController::_byteArray2Json(const QByteArray& data)
{

    QJsonParseError error;
    QString str = QString::fromUtf8(data.toBase64());
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if(error.error != QJsonParseError::NoError)
        qDebug() << error.errorString();
    return doc.object();
}

}

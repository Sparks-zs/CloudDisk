#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <QObject>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonArray>
#include <QSharedPointer>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QElapsedTimer>
#include <QFile>

#include "fileinfo.h"
#include "httpnetwork.h"


namespace CloudDisk {

class NetworkController : public QObject
{
    Q_OBJECT
public:
    typedef QSharedPointer<NetworkController> ControllerPtr;

    explicit NetworkController(QObject *parent = nullptr);
    ~NetworkController(){
        qDebug() << "networkcontroller delete";
    } 

    void upload(const QString& localFilePath, const QString& hostFilePath);
    void download(const QString& localFilePath, const QString& hostFilePath, qint64 hostFileSize);

    void startUpload(quint64 from, quint64 to);
    void startDownload(quint64 from, quint64 to);

    bool isUploading() { return _isUploading == true; }
    bool isDownloading() { return _isDownloading == true; }

    void setUploadSize(quint64 size) { _uploadSize = size; }
    void setDownloadSize(quint64 size) { _downloadSize = size; }

    void setFileInfo(const FileInfo& info) { _fileInfo = info; }
    FileInfo getFileInfo() { return _fileInfo; }

    double getUploadElapsedTime() { return _uploadElapsedTime / 1000; }
    double getDownloadElapsedTime() { return _downloadElapsedTime / 1000; }

public slots:
    void onfinished();
    void onReadyRead(const QByteArray& data);   // 接收server发来的内容

signals:
    void uploadFinished();
    void downloadFinished();
    void finished(const QJsonObject& json);
    void downloadProgress(quint64 bytesRecived, quint64 bytesTotal);
    void uploadProgress(quint64 bytesSent, quint64 bytesTotal);

private:
    QByteArray _read(quint64 from = 0, quint64 to = 0);
    quint64 _write(quint64 from, const QByteArray& data);
    void _handleUpload(const QJsonObject& json);
    void _handleDownload(const QByteArray& data);
    QByteArray _json2ByteArray(const QJsonObject& json);
    QJsonObject _byteArray2Json(const QByteArray& data);

    std::unique_ptr<HttpNetwork> _net;
    QString _url;
    QByteArray _data;

    bool _isDownloading, _isUploading;
    QFile _uploadFile, _downloadFile;
    QString _localFilePath;
    QString _hostFilePath;
    quint64 _hostFileSize;
    quint64 _uploadSize, _downloadSize;
    FileInfo _fileInfo;

    QElapsedTimer _uploadTimer;
    double _uploadElapsedTime;

    QElapsedTimer _downloadTimer;
    double _downloadElapsedTime;
};


}

#endif // NETWORKCONTROLLER_H

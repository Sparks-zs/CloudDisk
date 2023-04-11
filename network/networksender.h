#ifndef NETWORKSENDER_H
#define NETWORKSENDER_H

#include <QObject>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

#include "httpnetwork.h"

namespace CloudDisk{

class NetworkSender : public QObject
{
    Q_OBJECT
public:
    explicit NetworkSender(QObject *parent = nullptr);
    ~NetworkSender(){
        qDebug() << "NetworkSender delete";
    }

    void get(const QNetworkRequest& request);
    void post(const QNetworkRequest& request, const QByteArray& data);

    void getListDir(const QString& dirPath);
    void createDirectory(const QString& dirPath);
    void getFileInfo(const QString& filePath);
    void renameFile(const QString& filePath, const QString& newName);
    void removeFile(const QString& filePath);


signals:
    void finished(const QJsonObject& json);

public slots:
    void onfinished();
    void onReadyRead(const QByteArray& data);   // 接收server发来的内容

private:
    QByteArray _json2ByteArray(const QJsonObject& json);
    QJsonObject _byteArray2Json(const QByteArray& json);

    std::unique_ptr<HttpNetwork> _net;
    QString _url;
    QByteArray _data;

};

}

#endif // NETWORKSENDER_H

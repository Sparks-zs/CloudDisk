#include "networksender.h"
#include "../config.h"

namespace CloudDisk{

NetworkSender::NetworkSender(QObject *parent)
    : QObject{parent},
      _net(new HttpNetwork(this)),
      _url(URL)
{
    connect(_net.get(), &HttpNetwork::readyRead, this, &NetworkSender::onReadyRead);
    connect(_net.get(), &HttpNetwork::finished, this, &NetworkSender::onfinished);
}

void NetworkSender::get(const QNetworkRequest& request)
{
    _net->get(request);
}

void NetworkSender::post(const QNetworkRequest& request, const QByteArray& data)
{
    _net->post(request, data);
}


void NetworkSender::getListDir(const QString& dirPath)
{
    QNetworkRequest request(_url + "/filelist?path=" + dirPath);
    get(request);
}

void NetworkSender::createDirectory(const QString& dirPath)
{
    QNetworkRequest request(_url + "/create/directory?path=" + dirPath);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    post(request, "");
}

void NetworkSender::getFileInfo(const QString& filePath)
{
    QNetworkRequest request(_url + "/fileinfo?path=" + filePath);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    get(request);
}

void NetworkSender::renameFile(const QString& filePath, const QString& newName)
{
    QNetworkRequest request(_url + "/rename/file?path=" + filePath + "&newname=" + newName);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    post(request, "");
}

void NetworkSender::removeFile(const QString& filePath)
{
    QNetworkRequest request(_url + "/remove/file?path=" + filePath);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    post(request, "");
}

void NetworkSender::onfinished()
{
    emit finished(_byteArray2Json(_data));
    _data.clear();
}

void NetworkSender::onReadyRead(const QByteArray& data)
{

    _data.append(data);
}

QByteArray NetworkSender::_json2ByteArray(const QJsonObject& json)
{
    QJsonDocument doc(json);
    return doc.toJson(QJsonDocument::Compact);
}

QJsonObject NetworkSender::_byteArray2Json(const QByteArray& json)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    if(error.error != QJsonParseError::NoError)
        qDebug() << error.errorString();
    return doc.object();
}

}

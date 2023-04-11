#include "httpnetwork.h"

namespace CloudDisk{

QNetworkAccessManager* HttpNetwork::_manager = nullptr;
int HttpNetwork::_count = 0;

HttpNetwork::HttpNetwork(QObject* parent)
    : QObject(parent)
{
    if(!_manager){
        _manager = new QNetworkAccessManager(this);
    }

    _count++;
}

HttpNetwork::~HttpNetwork()
{
    _count--;
    if(_count == 0){
        _manager->deleteLater();
    }
    qDebug() << "httpnetwork delete";
}

void HttpNetwork::get(const QNetworkRequest& request)
{
    QNetworkReply* reply = _manager->get(request);
    _connectToReply(reply);
}

void HttpNetwork::post(const QNetworkRequest& request, const QByteArray& data)
{
    QNetworkReply* reply = _manager->post(request, data);
    _connectToReply(reply);
}

void HttpNetwork::onfinish()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    for(const QByteArray& header : reply->rawHeaderList()){
        _headers[header] = reply->rawHeader(header);
    }

    if(reply->isFinished()){
        emit finished();
    }
}

void HttpNetwork::onReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data = reply->readAll();
    emit readyRead(data);
}

void HttpNetwork::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{

}

void HttpNetwork::onDownloadProgress(qint64 bytesSent, qint64 bytesTotal)
{

}

void HttpNetwork::onError(QNetworkReply::NetworkError code)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << reply->errorString();
}

void HttpNetwork::_connectToReply(QNetworkReply* reply)
{
    connect(reply, &QIODevice::readyRead, this, &HttpNetwork::onReadyRead);
    connect(reply, &QNetworkReply::finished, this, &HttpNetwork::onfinish);
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
    connect(reply, &QNetworkReply::errorOccurred, this, &HttpNetwork::onError);
    connect(reply, &QNetworkReply::uploadProgress, this, &HttpNetwork::onUploadProgress);
    connect(reply, &QNetworkReply::downloadProgress, this, &HttpNetwork::onDownloadProgress);
    _headers.clear();
}

};

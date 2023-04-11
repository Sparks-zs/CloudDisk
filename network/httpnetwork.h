#ifndef HTTPNETWORK_H
#define HTTPNETWORK_H

#include <QMap>
#include <QObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

namespace CloudDisk{
class HttpNetwork : public QObject
{
    Q_OBJECT
public:
    HttpNetwork(QObject* parent = nullptr);
    ~HttpNetwork();

    void get(const QNetworkRequest& request);
    void post(const QNetworkRequest& request, const QByteArray& data);
    QByteArray getHeader(const QByteArray& header) const {
        if(!_headers.contains(header)) return {};
        return _headers[header];
    }

public slots:
    void onfinish();
    void onReadyRead();
    void onError(QNetworkReply::NetworkError code);
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void onDownloadProgress(qint64 bytesSent, qint64 bytesTotal);


signals:
    void finished();
    void readyRead(const QByteArray& data);
    void progress(qint64 bytesSent, qint64 bytesTotal);

private:
    void _connectToReply(QNetworkReply* reply);

    static QNetworkAccessManager* _manager;
    static int _count;
    QMap<QByteArray, QByteArray> _headers;
};

};
#endif // HTTPNETWORK_H

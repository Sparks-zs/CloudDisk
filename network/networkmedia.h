#ifndef NETWORKMEDIA_H
#define NETWORKMEDIA_H

#include <QObject>
#include <QBuffer>

class NetworkMedia : public QObject
{
    Q_OBJECT
public:
    explicit NetworkMedia(QObject *parent = nullptr);
    void setRecvBuffer(QBuffer* buffer);


signals:


private:
    QBuffer* _recvBuffer;
};

#endif // NETWORKMEDIA_H

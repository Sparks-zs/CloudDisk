#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QVector>

#include "networksender.h"
#include "networkcontroller.h"

namespace CloudDisk{

class NetworkManager : public QObject
{
    Q_OBJECT
public:

    static NetworkManager* instance() {
        if(!_instance){
            _instance = new NetworkManager;
        }
        return _instance;
    }
    ~NetworkManager(){

    }

    NetworkController* upload(const FileInfo& info, const QString& hostFilePath);
    NetworkController* download(const FileInfo& info, const QString& localFilepath);

    NetworkSender* get_listDir(const QString& path);
    NetworkSender* create_directory(const QString& path);
    NetworkSender* get_fileInfo(const QString& filePath);
    NetworkSender* renameFile(const QString& filePath, const QString& newName);
    NetworkSender* removeFile(const QString& filePath);

signals:
    void uploading_controller(NetworkController*);
    void downloading_controller(NetworkController*);

private:
    NetworkManager(QObject *parent = nullptr);
    static NetworkManager* _instance;

    QVector<NetworkController::ControllerPtr> _controllers;
};

}

#endif // NETWORKMANAGER_H

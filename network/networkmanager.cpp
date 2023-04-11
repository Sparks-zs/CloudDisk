#include <QJsonArray>
#include <QFileInfo>

#include "networkmanager.h"

namespace CloudDisk{

NetworkManager* NetworkManager::_instance = nullptr;

NetworkManager::NetworkManager(QObject *parent)
    : QObject{parent}
{

}

NetworkController* NetworkManager::upload(const FileInfo& info, const QString& hostFilepath)
{
    NetworkController::ControllerPtr controller(new NetworkController());
    controller->setFileInfo(info);
    controller->upload(info.path(), hostFilepath);

    _controllers.push_back(controller);
    emit uploading_controller(controller.get());
    return controller.get();
}

NetworkController* NetworkManager::download(const FileInfo& info, const QString& localFilepath)
{
    NetworkController::ControllerPtr controller(new NetworkController());
    controller->setFileInfo(info);
    controller->download(localFilepath, info.path(), info.size());

    _controllers.push_back(controller);
    emit downloading_controller(controller.get());
    return controller.get();
}

NetworkSender* NetworkManager::get_listDir(const QString& dirPath)
{
    NetworkSender* sender = new NetworkSender();
    sender->getListDir(dirPath);
    return sender;
}

NetworkSender* NetworkManager::create_directory(const QString& dirPath)
{
    NetworkSender* sender = new NetworkSender();
    sender->createDirectory(dirPath);
    return sender;
}

NetworkSender* NetworkManager::get_fileInfo(const QString& filePath)
{
    NetworkSender* sender = new NetworkSender();
    sender->getFileInfo(filePath);
    return sender;
}

NetworkSender* NetworkManager::renameFile(const QString& filePath, const QString& newName)
{
    NetworkSender* sender = new NetworkSender();
    sender->renameFile(filePath, newName);
    return sender;
}

NetworkSender* NetworkManager::removeFile(const QString& filePath)
{
    NetworkSender* sender = new NetworkSender();
    sender->removeFile(filePath);
    return sender;
}

}

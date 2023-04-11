#include <QFileDialog>
#include <QJsonArray>
#include <QInputDialog>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QCollator>
#include <algorithm>

#include "filewindow.h"
#include "ui_filewindow.h"
#include "playerwindow.h"
#include "../config.h"
#include "../widgets/flowlayout.h"
#include "../network/networkmanager.h"
#include "../network/networkcontroller.h"
#include "../network/fileinfo.h"

namespace CloudDisk{

FileWindow::FileWindow(const QString& rootDirPath, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileWindow),
    _rootDirPath(rootDirPath)
{
    ui->setupUi(this);
    init();
}

FileWindow::~FileWindow()
{
    delete ui;
    if(!_fileWidgets.empty()){
        for(FileWidget* widget : _fileWidgets){
            delete widget;
        }
    }
    if(!_folderWidgets.empty()){
        for(FileWidget* widget : _folderWidgets){
            delete widget;
        }
    }
}

void FileWindow::init()
{
    _layout = new FlowLayout;
    ui->filelistWidget->setLayout(_layout);

    QMenu* menu = new QMenu(this);
    QAction* uploadAction = new QAction("上传文件", menu);
    QAction* createDirAction = new QAction("新建文件夹", menu);
    connect(uploadAction, &QAction::triggered, this, &FileWindow::onUploadButtonClicked);
    connect(createDirAction, &QAction::triggered, this, &FileWindow::onCreateDirButtonClicked);

    menu->addAction(uploadAction);
    menu->addAction(createDirAction);
    menu->installEventFilter(this);
    menu->setAttribute(Qt::WA_TranslucentBackground);
    menu->setWindowFlags(menu->windowFlags()  | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint );

    ui->addButton->setMenu(menu);

    QFile qssFile(":/qss/filewindow/filewindow.qss");
    qssFile.open(QFile::ReadOnly);
    QString style = qssFile.readAll();
    this->setStyleSheet(style);

    update();
}

void FileWindow::update()
{
    getListdir(getCurDirPath());
    // 更新返回按键label
    QString dirName = _curDirPath.back();
    if(dirName == "./"){
        ui->backButton->setVisible(false);
        ui->curDirName->setText("");
    }
    else{
        ui->backButton->setVisible(true);
        ui->curDirName->setText(dirName);
    }
}

void FileWindow::addAndSortWidget()
{
    for(FileWidget* folder : _folderWidgets){
        _layout->addWidget(folder);
    }

    for(FileWidget* file : _fileWidgets){
        _layout->addWidget(file);
    }

}

void FileWindow::getListdir(const QString& dir_path)
{
    NetworkSender* sender = NetworkManager::instance()->get_listDir(dir_path);
    connect(sender, &NetworkSender::finished, this, &FileWindow::onGetListDir);
}

void FileWindow::onGetListDir(const QJsonObject& json)
{
    checkJson(json);

    if(!_folderWidgets.empty()){
        for(FileWidget* widget : _folderWidgets){
            _layout->removeWidget(widget);
            delete widget;
        }
        _folderWidgets.clear();
    }

    if(!_fileWidgets.empty()){
        for(FileWidget* widget : _fileWidgets){
            _layout->removeWidget(widget);
            delete widget;
        }
        _fileWidgets.clear();
    }

    QJsonArray filelist = json["data"].toArray();
    int size = filelist.size();

    for(int i = 0; i < size; i++){
        QJsonObject fileInfo = filelist[i].toObject();
        addFileWidget(fileInfo);
    }

    addAndSortWidget();
}

void FileWindow::addFileWidget(const QJsonObject& json)
{
    if(json.isEmpty()) return;

    bool isFolder = json["isfolder"].toBool();
    FileInfo info(json["name"].toString().toUtf8(),
                  json["path"].toString().toUtf8(),
                  isFolder,
                  json["size"].toInteger());

    FileWidget* fileWidget = new FileWidget(info);
    connectFileWidget(fileWidget);

    if(isFolder){
        if(_folderWidgets.contains(info.name())){
            _layout->removeWidget(_folderWidgets[info.name()]);
            delete _folderWidgets[info.name()];
        }
        _folderWidgets[info.name()] = fileWidget;
    }
    else {
        if(_fileWidgets.contains(info.name())){
            _layout->removeWidget(_fileWidgets[info.name()]);
            delete _fileWidgets[info.name()];
        }
        _fileWidgets[info.name()] = fileWidget;
    }
}

void FileWindow::onFileWidgetOpen()
{
    FileWidget* fileWidget = qobject_cast<FileWidget*>(sender());
    if(fileWidget->type() == FileInfo::FOLDER){
        addSubDirPath(fileWidget->info().name());
        update();
    }
    else if(fileWidget->type() == FileInfo::MP4){
        QString url = QString(URL) + "/video/file?path=" + fileWidget->info().path();
        PlayerWindow::getInstance()->addVideo(url);
    }
}

void FileWindow::onFileWidgetDownload()
{
    FileWidget* fileWidget = qobject_cast<FileWidget*>(sender());
    QString save_filepath = _rootDirPath + fileWidget->info().path();
    NetworkManager::instance()->download(fileWidget->info(), save_filepath);
}

void FileWindow::onFileWidgetRemove()
{
    FileWidget* fileWidget = qobject_cast<FileWidget*>(sender());
    QString path = getCurDirPath() + '/' + fileWidget->info().name();
    NetworkSender* sender = NetworkManager::instance()->removeFile(path);
    connect(sender, &NetworkSender::finished, this, &FileWindow::onRemove);
}

void FileWindow::onFileWidgetRename()
{
    FileWidget* fileWidget = qobject_cast<FileWidget*>(sender());
    QString path = getCurDirPath() + '/' + fileWidget->info().name();

    QString dlgTitle = "dialog";
    QString txtLabel = "重命名";
    QString iniInput = fileWidget->info().name();
    bool ok = false;
    QString newname = QInputDialog::getText(this, dlgTitle, txtLabel, QLineEdit::Normal, iniInput, &ok);
    if(ok && !newname.isEmpty()){
        NetworkSender* sender = NetworkManager::instance()->renameFile(path, newname);
        connect(sender, &NetworkSender::finished, this, &FileWindow::onRename);
    }

}

void FileWindow::onUpload()
{
    update();
}

void FileWindow::onDownload(const QJsonObject& json)
{
}

void FileWindow::onRemove(const QJsonObject& json)
{
    if(!checkJson(json)) return;
    update();
}

void FileWindow::onRename(const QJsonObject& json)
{
    if(!checkJson(json)) return;
    update();
}

void FileWindow::onCreateDir(const QJsonObject& json)
{
    if(!checkJson(json)) return;
    update();
}

void FileWindow::connectFileWidget(FileWidget* fileWidget)
{
    connect(fileWidget, &FileWidget::open, this, &FileWindow::onFileWidgetOpen);
    connect(fileWidget, &FileWidget::download, this, &FileWindow::onFileWidgetDownload);
    connect(fileWidget, &FileWidget::del, this, &FileWindow::onFileWidgetRemove);
    connect(fileWidget, &FileWidget::rename, this, &FileWindow::onFileWidgetRename);
}

QString FileWindow::getCurDirPath()
{
    if(_curDirPath.empty()){
        _curDirPath.push_back("./");
    }

    QString path;
    qsizetype size = _curDirPath.size();
    for(int i = 0; i < size; i++){
        path += _curDirPath[i];
        if(i + 1 < size && _curDirPath[i].back() != '/')
            path += '/';
    }
    return path;
}

QString FileWindow::getParentDirPath()
{
    if(_curDirPath.size() > 1){
        _curDirPath.pop_back();
    }

    return getCurDirPath();
}

void FileWindow::addSubDirPath(const QString& dirName)
{
    if(dirName.isEmpty()){
        return;
    }
    _curDirPath.push_back(dirName);
}

bool FileWindow::checkJson(const QJsonObject& json)
{
    if(json.empty()){
        return false;
    }

    if(json["code"] != 0){
        qDebug() << json["msg"];
        return false;
    }
    return true;
}

void FileWindow::onUploadButtonClicked()
{
    const QString filePath = QFileDialog::getOpenFileName(this, "选择文件进行上传或下载", "../");
    QString filename = filePath.split('/').last();
    if(filename.isEmpty()){
        return;
    }
    QFileInfo qinfo(filePath);
    QString hostFilePath = getCurDirPath() + '/' + filename;
    FileInfo info(filename, filePath, false, qinfo.size());
    NetworkController* controller = NetworkManager::instance()->upload(info, hostFilePath);
    connect(controller, &NetworkController::uploadFinished, this, &FileWindow::onUpload);
}

void FileWindow::onCreateDirButtonClicked()
{
    QString dlgTitle = "dialog";
    QString txtLabel = "please input new name of directory";
    QString iniInput = "new directory";
    bool ok = false;

    QString dirname = QInputDialog::getText(this, dlgTitle, txtLabel, QLineEdit::Normal, iniInput, &ok);
    if(ok && !dirname.isEmpty()){
        addSubDirPath(dirname);
        NetworkSender* sender = NetworkManager::instance()->create_directory(getCurDirPath());
        connect(sender, &NetworkSender::finished, this, &FileWindow::onCreateDir);
    }
}

void FileWindow::on_updateButton_clicked()
{
    //TODO 添加旋转效果
    update();
}

void FileWindow::on_backButton_clicked()
{
    getParentDirPath();
    update();
}

bool FileWindow::eventFilter(QObject* obj, QEvent* event)
{
    QMenu* menu = ui->addButton->menu();
    if (event->type() == QEvent::Show && obj == menu){
        QPoint point = ui->addButton->mapToGlobal(QPoint(0, 0));
        int button_width = ui->addButton->sizeHint().width();
        int button_height = ui->addButton->sizeHint().height();
        QPoint right_bottom_point = point + QPoint(button_width, button_height);
        menu->move(QPoint(right_bottom_point.x() - menu->width(),
                          right_bottom_point.y()));

        return true;
    }
    return false;

}

};

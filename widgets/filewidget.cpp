#include <QStyle>
#include <QIcon>
#include <QFile>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QFontMetrics>

#include "filewidget.h"
#include "ui_filewidget.h"

namespace CloudDisk{

FileWidget::FileWidget(const FileInfo& info, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileWidget),
    _type("txt"),
    _isPressed(false),
    _fileInfo(info)
{
    ui->setupUi(this);
    init();
}

FileWidget::~FileWidget()
{
    delete ui;
}

void FileWidget::init()
{
    setAttribute(Qt::WA_StyledBackground);
    //setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    this->layout()->setAlignment(Qt::AlignJustify);

    // 文件名过长则截断
    QString name = _fileInfo.name();
    ui->name->setText(name);
    QFontMetrics metrics(ui->name->font());
    if (name.size() > 10)
    {
        name = QFontMetrics(ui->name->font()).elidedText(name, Qt::ElideRight, 100);
    }
    ui->name->setText(name);

    switch(_fileInfo.type()){
    case FileInfo::FOLDER:
        _type = "folder";
        break;
    case FileInfo::TXT:
        _type = "txt";
        break;
    case FileInfo::PNG:
        _type = "png";
        break;
    case FileInfo::JPG:
        _type = "jpg";
        break;
    case FileInfo::MP4:
        _type = "mp4";
        break;
    case FileInfo::MKV:
        _type = "mkv";
        break;
    case FileInfo::ZIP:
        _type = "zip";
        break;
    default:
        _type = "unknown";
        break;
    }

    _menu = new QMenu(this);
    //_menu->setAttribute(Qt::WA_StyledBackground, true);
    //_menu->setAttribute(Qt::WA_TranslucentBackground);
    //_menu->setWindowFlags(Qt::FramelessWindowHint);

    QAction* openAction = new QAction("打开", this);
    QAction* downloadAction = new QAction("下载", this);
    QAction* delAction = new QAction("删除", this);
    QAction* renameAction = new QAction("重命名", this);
    QAction* moreInfoAction = new QAction("详细信息", this);

    connect(openAction, &QAction::triggered, this, &FileWidget::onOpenAction);
    connect(downloadAction, &QAction::triggered, this, &FileWidget::onDownloadAction);
    connect(delAction, &QAction::triggered, this, &FileWidget::onDelAction);
    connect(renameAction, &QAction::triggered, this, &FileWidget::onRenameAction);
    connect(moreInfoAction, &QAction::triggered, this, &FileWidget::onMoreInfoAction);

    _menu->addAction(openAction);
    _menu->addAction(downloadAction);
    _menu->addAction(delAction);
    _menu->addAction(renameAction);
    _menu->addAction(moreInfoAction);

    QFile qssFile(":/qss/file/file.qss");
    qssFile.open(QFile::ReadOnly);
    QString style = qssFile.readAll();
    this->setStyleSheet(style);
}

void FileWidget::onOpenAction()
{
    emit open();
}

void FileWidget::onDownloadAction()
{
    emit download();
}

void FileWidget::onDelAction()
{
    emit del();
}

void FileWidget::onRenameAction()
{
    emit rename();
}

void FileWidget::onMoreInfoAction()
{

}

void FileWidget::mousePressEvent(QMouseEvent* event)
{
    _isPressed = true;
    if(event->button() == Qt::RightButton)
        _menu->exec(cursor().pos());
    QWidget::mousePressEvent(event);
}

void FileWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){ 
        emit open();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void FileWidget::mouseReleaseEvent(QMouseEvent* event)
{
    _isPressed = false;
}

}

#include <QLabel>
#include <QProgressBar>
#include <QStandardItem>
#include <QFile>

#include "transferwindow.h"
#include "ui_transferwindow.h"
#include "../network/networkmanager.h"

namespace CloudDisk{

TransferWindow::TransferWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransferWindow)
{
    ui->setupUi(this);
    init();

    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->tabWidget->setAttribute(Qt::WA_TranslucentBackground);
    ui->tabWidget->setAttribute(Qt::WA_StyledBackground, true);
    ui->tabWidget->setWindowFlags(Qt::FramelessWindowHint);

    ui->uploadTab->setAttribute(Qt::WA_TranslucentBackground);
    ui->uploadTab->setAttribute(Qt::WA_StyledBackground, true);
    ui->uploadTab->setWindowFlags(Qt::FramelessWindowHint);

    QFile qssFile(":/qss/transferwindow/transferwindow.qss");
    qssFile.open(QFile::ReadOnly);
    QString style = qssFile.readAll();
    this->setStyleSheet(style);
}

TransferWindow::~TransferWindow()
{
    delete ui;
    for(auto& wid : _uploadControllerWidgets){
        delete wid;
    }
    for(auto& wid : _downloadControllerWidgets){
        delete wid;
    }
    for(auto& wid : _finishedControllerWidgets){
        delete wid;
    }
}

void TransferWindow::init()
{
    QStringList headerLabels(QString("文件名,速度,大小,状态").split(','));
    QStringList finishHeaderLabels(QString("文件名,大小,状态").split(','));
    // 正在上传界面
    QHeaderView* uploadHeader = ui->uploadTableWidget->horizontalHeader();
    uploadHeader->setSectionResizeMode(0, QHeaderView::Stretch);
    uploadHeader->setSectionResizeMode(1, QHeaderView::Fixed);
    uploadHeader->setSectionResizeMode(2, QHeaderView::Fixed);
    uploadHeader->setSectionResizeMode(3, QHeaderView::Fixed);
    uploadHeader->setHighlightSections(false);
    uploadHeader->setDefaultAlignment(Qt::AlignLeft);

    ui->uploadTableWidget->setHorizontalHeaderLabels(headerLabels);
    //ui->uploadTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->uploadTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->uploadTableWidget->setAttribute(Qt::WA_TranslucentBackground);
    ui->uploadTableWidget->setAttribute(Qt::WA_StyledBackground, true);
    ui->uploadTableWidget->setWindowFlags(Qt::FramelessWindowHint);

    // 正在下载界面
    QHeaderView* downloadHeader = ui->downloadTableWidget->horizontalHeader();
    downloadHeader->setSectionResizeMode(0, QHeaderView::Stretch);
    downloadHeader->setSectionResizeMode(1, QHeaderView::Fixed);
    downloadHeader->setSectionResizeMode(2, QHeaderView::Fixed);
    downloadHeader->setSectionResizeMode(3, QHeaderView::Fixed);
    downloadHeader->setHighlightSections(false);
    downloadHeader->setDefaultAlignment(Qt::AlignLeft);

    ui->downloadTableWidget->setHorizontalHeaderLabels(headerLabels);
    //ui->downloadTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->downloadTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 完成界面
    QHeaderView* finishedHeader = ui->finishedTableWidget->horizontalHeader();

    finishedHeader->setSectionResizeMode(0, QHeaderView::Stretch);
    finishedHeader->setSectionResizeMode(1, QHeaderView::Fixed);
    finishedHeader->setSectionResizeMode(2, QHeaderView::Fixed);
    finishedHeader->setHighlightSections(false);
    finishedHeader->setDefaultAlignment(Qt::AlignLeft);

    ui->finishedTableWidget->setHorizontalHeaderLabels(finishHeaderLabels);
    //ui->finishedTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->finishedTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(NetworkManager::instance(), &NetworkManager::uploading_controller,
            this, &TransferWindow::onUploadingController);
    connect(NetworkManager::instance(), &NetworkManager::downloading_controller,
            this, &TransferWindow::onDownloadingController);
}

void TransferWindow::onUploadingController(NetworkController* controller)
{
    int rowCount = ui->uploadTableWidget->rowCount();
    ui->uploadTableWidget->setRowCount(rowCount + 1);
    //ui->uploadTableWidget->insertRow(rowCount);

    connect(controller, &NetworkController::uploadFinished,
            this, &TransferWindow::onUploadControllerFinished);

    int row = _uploadControllerWidgets.size();
    NetworkControllerWidget* widget = new NetworkControllerWidget(controller, row);
    _uploadControllerWidgets[controller] = widget;

    QTableWidgetItem *fileIconName = new QTableWidgetItem(
                widget->icon(),
                widget->fileName());
    QLabel* speed = new QLabel(widget->speed(), ui->downloadTableWidget);
    connect(widget, &NetworkControllerWidget::speedChanged, speed, &QLabel::setText);
    QLabel* size = new QLabel(QString::number(widget->size(), 'g', 3) + widget->unit());
//    size->setAlignment(Qt::AlignCenter);

    QWidget* proWidget = new QWidget;
    QGridLayout* prolayout = new QGridLayout;
    QProgressBar* progressBar = widget->progressBar();
    prolayout->addWidget(progressBar);
    proWidget->setLayout(prolayout);

    ui->uploadTableWidget->setItem(row, 0, fileIconName);
    ui->uploadTableWidget->setCellWidget(row, 1, speed);
    ui->uploadTableWidget->setCellWidget(row, 2, size);
    ui->uploadTableWidget->setCellWidget(row, 3, proWidget);
}

void TransferWindow::onUploadControllerFinished()
{
    NetworkController* controller = qobject_cast<NetworkController*>(sender());
    NetworkControllerWidget* widget = _uploadControllerWidgets[controller];
    int uploadTableRow = widget->id();
    _uploadControllerWidgets.remove(controller);

    int fininsedTableRow = _finishedControllerWidgets.size();
    widget->setId(fininsedTableRow);
    _finishedControllerWidgets[controller] = widget;

    QTableWidgetItem *fileIconName = new QTableWidgetItem(
                widget->icon(),
                widget->fileName());
    QLabel* size = new QLabel(QString::number(widget->size(), 'g', 3) + widget->unit());
//    size->setAlignment(Qt::AlignCenter);
    QLabel* state = new QLabel("上传成功");
//    state->setAlignment(Qt::AlignCenter);

    ui->uploadTableWidget->removeRow(uploadTableRow);
    ui->uploadTableWidget->update();

    int rowCount = ui->finishedTableWidget->rowCount();
    ui->finishedTableWidget->setRowCount(rowCount + 1);
    //ui->finishedTableWidget->insertRow(rowCount);

    ui->finishedTableWidget->setItem(fininsedTableRow, 0, fileIconName);
    ui->finishedTableWidget->setCellWidget(fininsedTableRow, 1, size);
    ui->finishedTableWidget->setCellWidget(fininsedTableRow, 2, state);
}

void TransferWindow::onDownloadingController(NetworkController* controller)
{
    int rowCount = ui->downloadTableWidget->rowCount();
    ui->downloadTableWidget->setRowCount(rowCount + 1);
    //ui->downloadTableWidget->insertRow(rowCount);

    connect(controller, &NetworkController::downloadFinished,
            this, &TransferWindow::onDownloadControllerFinished);

    int row = _downloadControllerWidgets.size();
    NetworkControllerWidget* widget = new NetworkControllerWidget(controller, row);
    _downloadControllerWidgets[controller] = widget;

    QTableWidgetItem *fileIconName = new QTableWidgetItem(
                widget->icon(),
                widget->fileName());
    QLabel* speed = new QLabel(widget->speed(), ui->downloadTableWidget);
    connect(widget, &NetworkControllerWidget::speedChanged, speed, &QLabel::setText);
    QLabel* size = new QLabel(QString::number(widget->size(), 'g', 3) + widget->unit());
//    size->setAlignment(Qt::AlignCenter);

    QWidget* proWidget = new QWidget;
    QGridLayout* prolayout = new QGridLayout;
    QProgressBar* progressBar = widget->progressBar();
    prolayout->addWidget(progressBar);
    proWidget->setLayout(prolayout);

    ui->downloadTableWidget->setItem(row, 0, fileIconName);
    ui->downloadTableWidget->setCellWidget(row, 1, speed);
    ui->downloadTableWidget->setCellWidget(row, 2, size);
    ui->downloadTableWidget->setCellWidget(row, 3, proWidget);
}

void TransferWindow::onDownloadControllerFinished()
{

    NetworkController* controller = qobject_cast<NetworkController*>(sender());
    NetworkControllerWidget* widget = _downloadControllerWidgets[controller];
    int downloadTableRow = widget->id();
    _downloadControllerWidgets.remove(controller);

    int fininsedTableRow = _finishedControllerWidgets.size();
    widget->setId(fininsedTableRow);
    _finishedControllerWidgets[controller] = widget;

    QTableWidgetItem *fileIconName = new QTableWidgetItem(
                widget->icon(),
                widget->fileName());

    QLabel* size = new QLabel(QString::number(widget->size(), 'g', 3) + widget->unit());
//    size->setAlignment(Qt::AlignCenter);
    QLabel* state = new QLabel("下载成功");
//    state->setAlignment(Qt::AlignCenter);

    ui->downloadTableWidget->removeRow(downloadTableRow);
    ui->downloadTableWidget->update();

    int rowCount = ui->finishedTableWidget->rowCount();
    ui->finishedTableWidget->setRowCount(rowCount + 1);
    //ui->finishedTableWidget->insertRow(rowCount);

    ui->finishedTableWidget->setItem(fininsedTableRow, 0, fileIconName);
    ui->finishedTableWidget->setCellWidget(fininsedTableRow, 1, size);
    ui->finishedTableWidget->setCellWidget(fininsedTableRow, 2, state);
}

void TransferWindow::onUploadingTabClicked()
{

}

void TransferWindow::onDownloadingTabClicked()
{

}

void TransferWindow::onFinishedTabClicked()
{
}

}

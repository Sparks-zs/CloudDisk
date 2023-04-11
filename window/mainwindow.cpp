#include <QMouseEvent>
#include <QFile>
#include <QThread>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filewindow.h"
#include "transferwindow.h"
#include "playerwindow.h"

#include "../network/networkmanager.h"

namespace CloudDisk{

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI()
{
    QThread* thread = new QThread();
    NetworkManager::instance()->moveToThread(thread);
    thread->start();

    QThread* thread2 = new QThread();
    PlayerWindow::getInstance()->moveToThread(thread2);
    thread2->start();

    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->topBar->installEventFilter(this);
    ui->stackedWidget->addWidget(new FileWindow("D:/myGitProjects/Qt/CloudDisk/download", this));
    ui->stackedWidget->addWidget(new TransferWindow(this));
    ui->stackedWidget->addWidget(PlayerWindow::getInstance());
    ui->stackedWidget->setCurrentIndex(0);

    QFile qssFile(":/qss/mainwindow/qss.qss");
    qssFile.open(QFile::ReadOnly);
    QString style = qssFile.readAll();

    this->setStyleSheet(style);
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

    if(obj == ui->topBar){
        if(mouseEvent->type() == QMouseEvent::MouseButtonPress){
            if(mouseEvent->button() == Qt::LeftButton){
                _isPressed = true;
                _pressPoint = mouseEvent->globalPosition();
            }
        }
        else if(mouseEvent->type() == QMouseEvent::MouseButtonRelease){
            _isPressed = true;
        }
        else if(mouseEvent->type() == QMouseEvent::MouseMove){
            if(_isPressed){
                QPointF relativePoint = mouseEvent->globalPosition() - _pressPoint;
                QPointF movePoint = this->pos() + relativePoint;
                move(movePoint.x(), movePoint.y());
                _pressPoint = mouseEvent->globalPosition().toPoint();
            }
        }
    }
//    if(_isPressed && event->type() == QEvent::WindowStateChange){
//        if(this->windowState() == Qt::WindowMaximized){
//            this->setWindowState(Qt::WindowActive);
//        }
//    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::changeEvent(QEvent *event)
{
    if(QEvent::WindowStateChange == event->type()){
        QWindowStateChangeEvent * stateEvent = dynamic_cast<QWindowStateChangeEvent*>(event);
        if(stateEvent){
            Qt::WindowStates state = stateEvent->oldState();
//            if(state == Qt::WindowMinimized){
//                ui->minButton->setEnabled(false);
//                ui->maxButton->setEnabled(true);
//            }
//            else if(state == Qt::WindowMaximized){
//                ui->minButton->setEnabled(true);
//                ui->maxButton->setEnabled(false);
//            }
//            else{
//                ui->minButton->setEnabled(true);
//                ui->maxButton->setEnabled(true);
//            }
        }
    }


}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
//    if(event->button() == Qt::LeftButton){
//        _isPressed = true;
//        _pressPoint = event->globalPosition();
//    }
    return QWidget::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    //_isPressed = false;
    return QWidget::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
//    if(!_isPressed){
//        event->ignore();
//        return;
//    }

//    QPointF relativePoint = event->globalPosition() - _pressPoint;
//    QPointF movePoint = this->pos() + relativePoint;
//    move(movePoint.x(), movePoint.y());
//    _pressPoint = event->globalPosition().toPoint();

    return QWidget::mouseMoveEvent(event);
}

void MainWindow::on_minButton_clicked()
{
    this->showMinimized();
}

void MainWindow::on_maxButton_clicked()
{
    this->showMaximized();
}

void MainWindow::on_closeButton_clicked()
{
    this->close();
}


void MainWindow::on_fileButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_transferButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_playerButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


};

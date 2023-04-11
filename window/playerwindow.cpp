#include "playerwindow.h"
#include "ui_playerwindow.h"
#include "../config.h"
#include "../network/networkmanager.h"

#include <QDir>
#include <QFileDialog>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QListWidgetItem>

namespace CloudDisk{
PlayerWindow* PlayerWindow::_instance = nullptr;

PlayListItem::PlayListItem(const FileInfo& info, QWidget* parent)
    : QWidget(parent), _info(info)
{
    init();
}

void PlayListItem::init()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(new QLabel(_info.name()));
    setLayout(layout);
}

PlayerWindow::PlayerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerWindow),
    _url(URL)
{
    ui->setupUi(this);
    init();
}

PlayerWindow::~PlayerWindow()
{
    delete ui;
}

void PlayerWindow::init()
{
    _player = new QMediaPlayer(this);
    QAudioOutput* audioOutput = new QAudioOutput(this);
    _player->setAudioOutput(audioOutput);
    _player->setVideoOutput(ui->videoWidget);

//    _loadBuffer = new QBuffer(this);
//    _loadBuffer->open(QBuffer::ReadWrite);
//    _player->setSourceDevice(_loadBuffer);

    ui->videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
    on_volumeSlider_valueChanged(20);

    connect(_player, &QMediaPlayer::playbackStateChanged, this, &PlayerWindow::do_stateChanged);
    connect(_player, &QMediaPlayer::positionChanged, this, &PlayerWindow::do_postionChanged);
    connect(_player, &QMediaPlayer::durationChanged, this, &PlayerWindow::do_durationChanged);
    connect(_player, &QMediaPlayer::mediaStatusChanged, this, &PlayerWindow::onMediaStatusChanged);

    connect(ui->playListWidget, &QListWidget::itemClicked, this, &PlayerWindow::onPlayListItemClicked);
    connect(ui->playListWidget, &QListWidget::itemDoubleClicked, this, &PlayerWindow::onPlayListItemDoubleClicked);

    QFile qssFile(":/qss/playerwindow/playerwindow.qss");
    qssFile.open(QFile::ReadOnly);
    QString style = qssFile.readAll();

    this->setStyleSheet(style);
}

void PlayerWindow::addVideoFile(const FileInfo& info)
{
    QListWidgetItem* item = new QListWidgetItem(ui->playListWidget);
    item->setSizeHint(QSize(80, 40));
    ui->playListWidget->addItem(item);
    ui->playListWidget->setItemWidget(item, new PlayListItem(info, ui->playListWidget));
    onPlayListItemDoubleClicked(item);
}

//播放器状态变化
void PlayerWindow::do_stateChanged(QMediaPlayer::PlaybackState state)
{
    bool isPlaying = (state==QMediaPlayer::PlayingState);
    ui->playButton->setEnabled(!isPlaying);
    ui->pauseButton->setEnabled(isPlaying);
    ui->stopButton->setEnabled(isPlaying);
}

//文件时长变化
void PlayerWindow::do_durationChanged(qint64 duration)
{
    ui->videoPositionSlider->setMaximum(duration);

    int   secs=duration/1000;   //秒
    int   mins=secs/60;         //分钟
    secs=secs % 60;             //余数秒
    _durationTime=QString::asprintf("%d:%d", mins, secs);
    ui->rationLabel->setText(_positionTime + "/" + _durationTime);
}

//文件播放位置变化
void PlayerWindow::do_postionChanged(qint64 position)
{
    if (ui->videoPositionSlider->isSliderDown())
        return;     //如果正在拖动滑条，退出
    ui->videoPositionSlider->setSliderPosition(position);
    int secs = position/1000;   //秒
    int mins = secs/60;         //分钟
    secs = secs % 60;             //余数秒
    _positionTime = QString::asprintf("%d:%d",mins,secs);
    ui->rationLabel->setText(_positionTime + "/" + _durationTime);

}

//打开文件
void PlayerWindow::on_openButton_clicked()
{
    QString curPath = QDir::homePath();
    QString dlgTitle = "选择视频文件";
    QString filter = "视频文件(*.wmv, *.mp4);;所有文件(*.*)";
    QString videoFile = QFileDialog::getOpenFileName(this, dlgTitle, curPath, filter);

    if (videoFile.isEmpty())
      return;

    QFileInfo fileInfo(videoFile);
    ui->curVideoName->setText(fileInfo.fileName());

    //_player->setSource(QUrl::fromLocalFile(videoFile));  //设置播放文件
    QFile videoFilet(videoFile);
    if(videoFilet.open(QFile::ReadOnly)){
        _loadBuffer->write(videoFilet.readAll());
    }
}

void PlayerWindow::on_playButton_clicked()
{
    _player->play();
}

void PlayerWindow::on_pauseButton_clicked()
{
    _player->pause();
}


void PlayerWindow::on_stopButton_clicked()
{
    _player->stop();
}


void PlayerWindow::on_volumeSlider_valueChanged(int value)
{
    _player->audioOutput()->setVolume(value / 100.0);
}


void PlayerWindow::on_videoPositionSlider_valueChanged(int value)
{
    _player->setPosition(value);
}

void PlayerWindow::on_fullScreenButton_clicked()
{
    ui->videoWidget->setFullScreen(true);
}

void PlayerWindow::onPlayListItemClicked(QListWidgetItem* item)
{
    PlayListItem* playItem = static_cast<PlayListItem*>(ui->playListWidget->itemWidget(item));
    QString url = playItem->filePath() + '/' + playItem->filename();

}

void PlayerWindow::onPlayListItemDoubleClicked(QListWidgetItem* item)
{
    PlayListItem* playItem = static_cast<PlayListItem*>(ui->playListWidget->itemWidget(item));
    QString path = playItem->filePath();
    QString url = _url + "/video/file?path=" + path;
    if(_player->isPlaying()){
        _player->stop();
    }
    ui->curVideoName->setText(playItem->filename());

    _player->setSource(url);
    // 开始下载视频

}

void PlayerWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if(status == QMediaPlayer::LoadedMedia){
        _player->play();
    }
}


}

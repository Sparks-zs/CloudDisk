#include "playerwindow.h"
#include "./ui_playerwindow.h"
#include "../config.h"
#include "../network/networkmanager.h"

#include <QDir>
#include <QList>
#include <QFileDialog>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QHBoxLayout>
#include <QStandardPaths>
#include <QMessageBox>

#include <QThread>

namespace CloudDisk{
PlayerWindow* PlayerWindow::_instance = nullptr;

PlayerWindow::PlayerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerWindow),
    _url(URL)
{
    ui->setupUi(this);
    init();

    QThread* thread = new QThread();
    _player->moveToThread(thread);
    thread->start();
}

PlayerWindow::~PlayerWindow()
{
    delete ui;
}

void PlayerWindow::init()
{
    _player = new QMediaPlayer();
    _audioOutput = new QAudioOutput(this);
    _player->setAudioOutput(_audioOutput);

    connect(_player, &QMediaPlayer::durationChanged, this, &PlayerWindow::durationChanged);
    connect(_player, &QMediaPlayer::positionChanged, this, &PlayerWindow::positionChanged);
    connect(_player, &QMediaPlayer::mediaStatusChanged, this, &PlayerWindow::statusChanged);
    connect(_player, &QMediaPlayer::bufferProgressChanged, this, &PlayerWindow::bufferingProgress);
    connect(_player, &QMediaPlayer::hasVideoChanged, this, &PlayerWindow::videoAvailableChanged);
    connect(_player, &QMediaPlayer::errorChanged, this, &PlayerWindow::displayErrorMessage);

    _videoWidget = new VideoWidget(this);
    _videoWidget->resize(1280, 720);
    _player->setVideoOutput(_videoWidget);

    _playlistModel = new PlaylistModel(this);
    _playlist = _playlistModel->playlist();
    connect(_playlist, &QMediaPlaylist::currentIndexChanged, this,
            &PlayerWindow::playlistPositionChanged);

    ui->displayLayout->addWidget(_videoWidget, 2);
    _playlistView = new QListView();
    _playlistView->setModel(_playlistModel);
    _playlistView->setCurrentIndex(_playlistModel->index(_playlist->currentIndex(), 0));
    connect(_playlistView, &QAbstractItemView::activated, this, &PlayerWindow::jump);
    ui->displayLayout->addWidget(_playlistView);

    // duration slider and label
    ui->videoPositionSlider->setRange(0, _player->duration());
    connect(ui->videoPositionSlider, &QSlider::sliderMoved, this, &PlayerWindow::seek);

    PlayerControls* controls = new PlayerControls();
    controls->setState(_player->playbackState());
    controls->setVolume(_audioOutput->volume());
    controls->setMuted(controls->isMuted());

    connect(controls, &PlayerControls::play, _player, &QMediaPlayer::play);
    connect(controls, &PlayerControls::pause, _player, &QMediaPlayer::pause);
    connect(controls, &PlayerControls::stop, _player, &QMediaPlayer::stop);
    connect(controls, &PlayerControls::next, _playlist, &QMediaPlaylist::next);
    connect(controls, &PlayerControls::previous, this, &PlayerWindow::previousClicked);
    connect(controls, &PlayerControls::changeVolume, _audioOutput, &QAudioOutput::setVolume);
    connect(controls, &PlayerControls::changeMuting, _audioOutput, &QAudioOutput::setMuted);
    connect(controls, &PlayerControls::changeRate, _player, &QMediaPlayer::setPlaybackRate);
    connect(controls, &PlayerControls::stop, _videoWidget, QOverload<>::of(&QVideoWidget::update));
    ui->controlLayout->addWidget(controls);

    connect(_player, &QMediaPlayer::playbackStateChanged, controls, &PlayerControls::setState);
    connect(_audioOutput, &QAudioOutput::volumeChanged, controls, &PlayerControls::setVolume);
    connect(_audioOutput, &QAudioOutput::mutedChanged, controls, &PlayerControls::setMuted);


    if (!isPlayerAvailable()) {
        QMessageBox::warning(this, tr("Service not available"),
                             tr("The QMediaPlayer object does not have a valid service.\n"
                                "Please check the media service plugins are installed."));
        controls->setEnabled(false);
        if (_playlistView)
            _playlistView->setEnabled(false);
        ui->openButton->setEnabled(false);
        ui->fullScreenButton->setEnabled(false);
    }

    QFile qssFile(":/qss/playerwindow/playerwindow.qss");
    qssFile.open(QFile::ReadOnly);
    QString style = qssFile.readAll();

    this->setStyleSheet(style);
}

bool PlayerWindow::isPlayerAvailable() const
{
    return _player->isAvailable();
}

void PlayerWindow::addVideo(const QString& url)
{
    QList<QUrl> urls{url};
    addToPlaylist(urls);
}

//文件播放位置变化
void PlayerWindow::playlistPositionChanged(int currentItem)
{
    if (_playlistView)
        _playlistView->setCurrentIndex(_playlistModel->index(currentItem, 0));
    _player->setSource(_playlist->currentMedia());

}

static bool isPlaylist(const QUrl &url) // Check for ".m3u" playlists.
{
    if (!url.isLocalFile())
        return false;
    const QFileInfo fileInfo(url.toLocalFile());
    return fileInfo.exists()
            && !fileInfo.suffix().compare(QLatin1String("m3u"), Qt::CaseInsensitive);
}

void PlayerWindow::addToPlaylist(const QList<QUrl> &urls)
{
    const int previousMediaCount = _playlist->mediaCount();
    for (auto &url : urls) {
        if (isPlaylist(url))
            _playlist->load(url);
        else
            _playlist->addMedia(url);
    }
    if (_playlist->mediaCount() > previousMediaCount) {
        auto index = _playlistModel->index(previousMediaCount, 0);
        if (_playlistView)
            _playlistView->setCurrentIndex(index);
        jump(index);
    }
}

void PlayerWindow::durationChanged(qint64 duration)
{
    _duration = duration / 1000;
    ui->videoPositionSlider->setMaximum(duration);
}

void PlayerWindow::positionChanged(qint64 progress)
{
    if (!ui->videoPositionSlider->isSliderDown())
        ui->videoPositionSlider->setValue(progress);

    updateDurationInfo(progress / 1000);
}

void PlayerWindow::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || _duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60, currentInfo % 60,
                          (currentInfo * 1000) % 1000);
        QTime totalTime((_duration / 3600) % 60, (_duration / 60) % 60, _duration % 60,
                        (_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    ui->rationLabel->setText(tStr);
}

void PlayerWindow::open()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Files"));
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation)
                                    .value(0, QDir::homePath()));
    if (fileDialog.exec() == QDialog::Accepted)
        addToPlaylist(fileDialog.selectedUrls());
}

void PlayerWindow::seek(int mseconds)
{
    _player->setPosition(mseconds);
}

void PlayerWindow::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        _playlist->setCurrentIndex(index.row());
    }
}

void PlayerWindow::previousClicked()
{
    // Go to previous track if we are within the first 5 seconds of playback
    // Otherwise, seek to the beginning.
    if (_player->position() <= 5000) {
        _playlist->previous();
    } else {
        _player->setPosition(0);
    }
}

void PlayerWindow::statusChanged(QMediaPlayer::MediaStatus status)
{
    handleCursor(status);

    // handle status message
    switch (status) {
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::LoadedMedia:
        //setStatusInfo(QString());
        break;
    case QMediaPlayer::LoadingMedia:
        //setStatusInfo(tr("Loading..."));
        break;
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
        //setStatusInfo(tr("Buffering %1%").arg(qRound(m_player->bufferProgress() * 100.)));
        break;
    case QMediaPlayer::StalledMedia:
        //setStatusInfo(tr("Stalled %1%").arg(qRound(m_player->bufferProgress() * 100.)));
        break;
    case QMediaPlayer::EndOfMedia:
        QApplication::alert(this);
        _playlist->next();
        break;
    case QMediaPlayer::InvalidMedia:
        //displayErrorMessage();
        break;
    }
}

void PlayerWindow::handleCursor(QMediaPlayer::MediaStatus status)
{
#ifndef QT_NO_CURSOR
    if (status == QMediaPlayer::LoadingMedia || status == QMediaPlayer::BufferingMedia
        || status == QMediaPlayer::StalledMedia)
        setCursor(QCursor(Qt::BusyCursor));
    else
        unsetCursor();
#endif
}

void PlayerWindow::bufferingProgress(float progress)
{
//    if (_player->mediaStatus() == QMediaPlayer::StalledMedia)
//        setStatusInfo(tr("Stalled %1%").arg(qRound(progress * 100.)));
//    else
//        setStatusInfo(tr("Buffering %1%").arg(qRound(progress * 100.)));
}

void PlayerWindow::videoAvailableChanged(bool available)
{
    if (!available) {
        disconnect(ui->fullScreenButton, &QPushButton::clicked, _videoWidget,
                   &QVideoWidget::setFullScreen);
        disconnect(_videoWidget, &QVideoWidget::fullScreenChanged, ui->fullScreenButton,
                   &QPushButton::setChecked);
        _videoWidget->setFullScreen(false);
    } else {
        connect(ui->fullScreenButton, &QPushButton::clicked, _videoWidget,
                &QVideoWidget::setFullScreen);
        connect(_videoWidget, &QVideoWidget::fullScreenChanged, ui->fullScreenButton,
                &QPushButton::setChecked);

        if (ui->fullScreenButton->isChecked())
            _videoWidget->setFullScreen(true);
    }
}

void PlayerWindow::displayErrorMessage()
{
    if (_player->error() == QMediaPlayer::NoError)
        return;
    //setStatusInfo(_player->errorString());
}

void PlayerWindow::on_openButton_clicked()
{
    open();
}

}

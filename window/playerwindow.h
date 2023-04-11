#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QWidget>
#include <QMediaPlayer>
#include <QListWidget>
#include <QBuffer>

#include "../network/fileinfo.h"

namespace Ui {
class PlayerWindow;
}

namespace CloudDisk{

class PlayListItem : public QWidget
{
    Q_OBJECT
public:
    PlayListItem(const FileInfo& info, QWidget* parent=nullptr);
    QString filename() { return _info.name(); }
    QString filePath() { return _info.path(); }
    FileInfo info() { return _info; }
private:
    void init();

    FileInfo _info;
};


class PlayerWindow : public QWidget
{
    Q_OBJECT

public:
    static PlayerWindow* getInstance(){
        if(!_instance){
            _instance = new PlayerWindow();
        }
        return _instance;
    }

    ~PlayerWindow();

    void init();
    void addVideoFile(const FileInfo& info);

private slots:
    void do_stateChanged(QMediaPlayer::PlaybackState state);
    void do_durationChanged(qint64 duration);
    void do_postionChanged(qint64 position);

    void on_openButton_clicked();
    void on_playButton_clicked();
    void on_pauseButton_clicked();
    void on_stopButton_clicked();
    void on_fullScreenButton_clicked();

    void on_volumeSlider_valueChanged(int value);
    void on_videoPositionSlider_valueChanged(int value);

    void onPlayListItemClicked(QListWidgetItem* item);
    void onPlayListItemDoubleClicked(QListWidgetItem* item);

    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
    static PlayerWindow* _instance;
    explicit PlayerWindow(QWidget *parent = nullptr);

    Ui::PlayerWindow *ui;
    QString _url;
    QMediaPlayer* _player;
    QString _durationTime;
    QString _positionTime;

    QBuffer* _loadBuffer;
};

}
#endif // PLAYERWINDOW_H

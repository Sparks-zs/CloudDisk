#ifndef PLAYERWINDOW_H
#define PLAYERWINDOW_H

#include <QWidget>
#include <QMediaPlayer>
#include <QListWidget>
#include <QAbstractItemView>

#include "../network/fileinfo.h"
#include "../media/videowidget.h"
#include "../media/playlistmodel.h"
#include "../media/qmediaplaylist.h"
#include "../media/playercontrols.h"

namespace Ui {
class PlayerWindow;
}

namespace CloudDisk{

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
    bool isPlayerAvailable() const;
    void addVideo(const QString& url);
    void addToPlaylist(const QList<QUrl> &urls);

private slots:
    void open();
    void seek(int mseconds);
    void jump(const QModelIndex &index);
    void playlistPositionChanged(int currentItem);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void previousClicked();

    void statusChanged(QMediaPlayer::MediaStatus status);
    void bufferingProgress(float progress);
    void videoAvailableChanged(bool available);
    void displayErrorMessage();

    void on_openButton_clicked();
private:
    static PlayerWindow* _instance;
    explicit PlayerWindow(QWidget *parent = nullptr);
    void updateDurationInfo(qint64 currentInfo);
    void handleCursor(QMediaPlayer::MediaStatus status);

    Ui::PlayerWindow *ui;
    QString _url;

    QAudioOutput* _audioOutput;
    QMediaPlayer* _player;
    VideoWidget* _videoWidget;
    PlaylistModel* _playlistModel;
    QMediaPlaylist* _playlist;
    QAbstractItemView* _playlistView;

    qint64 _duration;
};

}
#endif // PLAYERWINDOW_H

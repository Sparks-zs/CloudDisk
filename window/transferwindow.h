#ifndef TRANSFERWINDOW_H
#define TRANSFERWINDOW_H

#include <QWidget>
#include <QMap>
#include <QList>
#include <QVBoxLayout>
#include <QGridLayout>

#include "../network/networkcontroller.h"
#include "../widgets/networkcontrollerwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class TransferWindow;
}
QT_END_NAMESPACE

namespace CloudDisk{

class TransferWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TransferWindow(QWidget *parent = nullptr);
    ~TransferWindow();
    void init();

public slots:
    void onUploadingTabClicked();
    void onDownloadingTabClicked();
    void onFinishedTabClicked();

    void onUploadingController(NetworkController* controller);
    void onDownloadingController(NetworkController* controller);
    void onUploadControllerFinished();
    void onDownloadControllerFinished();

private:
    Ui::TransferWindow *ui;

    QMap<NetworkController*, NetworkControllerWidget*> _uploadControllerWidgets;
    QMap<NetworkController*, NetworkControllerWidget*> _downloadControllerWidgets;
    QMap<NetworkController*, NetworkControllerWidget*> _finishedControllerWidgets;
};

}

#endif // TRANSFERWINDOW_H

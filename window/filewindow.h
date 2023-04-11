#ifndef FILEWINDOW_H
#define FILEWINDOW_H

#include <QMap>
#include <QWidget>
#include <QToolButton>
#include <QStringList>

#include "../widgets/filewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class FileWindow;
}
QT_END_NAMESPACE

namespace CloudDisk{
class FileName{
public:
    FileName(const QString &name) : _name(name) {}
    //重载操作符
    bool operator < (const FileName& other) const
    {
        return _name < other._name;
    }

    QString _name;
};

class FileWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FileWindow(const QString& rootDirPath, QWidget *parent = nullptr);
    ~FileWindow();

    void init();
    void update();
    void getListdir(const QString& dir_path);
    void addAndSortWidget();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

    // 处理服务器的回应
public slots:
    void onUpload();
    void onGetListDir(const QJsonObject& json);
    void addFileWidget(const QJsonObject& json);
    void onDownload(const QJsonObject& json);
    void onRemove(const QJsonObject& json);
    void onRename(const QJsonObject& json);
    void onCreateDir(const QJsonObject& json);

private slots:
    void on_updateButton_clicked();
    void on_backButton_clicked();
    void onUploadButtonClicked();
    void onCreateDirButtonClicked();

    // 处理FileWidget发出的信号
    void onFileWidgetOpen();
    void onFileWidgetDownload();
    void onFileWidgetRemove();
    void onFileWidgetRename();

private:
    QString getCurDirPath();
    QString getParentDirPath();
    void addSubDirPath(const QString& dirName);
    void connectFileWidget(FileWidget* fileWidget);
    bool checkJson(const QJsonObject& json);

    Ui::FileWindow *ui;
    QLayout* _layout;

    QMap<const QString, FileWidget*> _folderWidgets;
    QMap<const QString, FileWidget*> _fileWidgets;
    QStringList _curDirPath;
    QString _rootDirPath;
};

};

#endif // FILEWINDOW_H

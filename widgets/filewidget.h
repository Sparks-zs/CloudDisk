#ifndef FILEWIDGET_H
#define FILEWIDGET_H

#include <QWidget>
#include <QMenu>
#include <QLabel>

#include "../network/fileinfo.h"

namespace Ui {
class FileWidget;
}
namespace CloudDisk{

class FileWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString filetype READ typeString CONSTANT)
    Q_PROPERTY(bool ispressed READ isPressed)

public:
    explicit FileWidget(const FileInfo& info, QWidget *parent = nullptr);
    ~FileWidget();

    void init();
    const FileInfo info() const { return _fileInfo; }
    int type() { return _fileInfo.type(); }

    // 为属性提供访问的函数
    QString typeString() { return _type; }
    bool isPressed() { return _isPressed; }

public slots:
    void onOpenAction();
    void onDownloadAction();
    void onDelAction();
    void onRenameAction();
    void onMoreInfoAction();

signals:
    void open();
    void download();
    void del();
    void rename();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    Ui::FileWidget *ui;
    QString _type;
    bool _isPressed;

    FileInfo _fileInfo;
    QMenu* _menu;


};

}

#endif // FILEWIDGET_H

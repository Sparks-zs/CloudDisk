#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace CloudDisk{
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initUI();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void changeEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private slots:
    void on_closeButton_clicked();

    void on_maxButton_clicked();

    void on_minButton_clicked();

    void on_transferButton_clicked();

    void on_fileButton_clicked();

    void on_playerButton_clicked();

private:
    Ui::MainWindow *ui;
    bool _isPressed;
    QPointF _pressPoint;
};
};
#endif // MAINWINDOW_H

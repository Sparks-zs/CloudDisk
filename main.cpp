#include "window/mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

using namespace CloudDisk;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QTranslator translator;
//    const QStringList uiLanguages = QLocale::system().uiLanguages();
//    for (const QString &locale : uiLanguages) {
//        const QString baseName = "CloudDisk_" + QLocale(locale).name();
//        if (translator.load(":/i18n/" + baseName)) {
//            a.installTranslator(&translator);
//            break;
//        }
//    }

    MainWindow w;
    w.show();
    return a.exec();
}

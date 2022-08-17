#include "mainwindow.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    QTimer::singleShot(200, [&a](){ a.exit(0);});


    return a.exec();
}

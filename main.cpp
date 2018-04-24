#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("plastique"));
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}

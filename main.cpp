#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
//1. bug with addaing to proxy model
//2. public/private in cart.h
//3. how to fix the scale of widgets
//4 how to add buttons to tableview

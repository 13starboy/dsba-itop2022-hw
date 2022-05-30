#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
//1. problrm with sort because of Big later + numbers sort like strings + how better to return default data
//2. public/private in cart.h
//3. inheretation from QWidget or MainWindow in cart.h/cart.cpp
//4. how to fix the scale of widgets
//5. how to add buttons to tableview
//6. intializing data: how to hide column without loosing data


#ifndef ADD_H
#define ADD_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class add;
}

class add : public QDialog
{
    Q_OBJECT

public:
    explicit add(QWidget *parent = nullptr);
    ~add();

signals:
    void sendData(Item& res);

private slots:
    void on_add_button_clicked();

private:
    Ui::add *ui;
};

#endif // ADD_H

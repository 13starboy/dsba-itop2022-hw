#include "add.h"
#include "ui_add.h"
#include "mainwindow.h"

add::add(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::add)
{
    ui->setupUi(this);
}

add::~add()
{
    delete ui;
}

void add::on_add_button_clicked()
{
    try
    {
        Item res;
        res.data[0] = QString(ui->name_line->text().toStdString().c_str());
        res.data[1] = QString::number(std::stod(ui->price_line->text().toStdString()), 'd', 2);
        emit sendData(res);
        close();
    }
    catch (...)
    {
        close();
    }
}


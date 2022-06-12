#include "add.h"
#include "mainwindow.h"
#include <QGridLayout>

add::add(QWidget* parent) : QDialog(parent)
{
    QGridLayout* l = new QGridLayout(this);
    QHBoxLayout* price = new QHBoxLayout();
    QHBoxLayout* name = new QHBoxLayout();

    name_line = new QLineEdit();
    price_line = new QLineEdit();
    price_lb = new QLabel();
    name_lb = new QLabel();
    apply = new QPushButton();

    name_lb->setText("Name of Product");
    price_lb->setText("Price of Product");
    apply->setText("Apply");
    name_lb->setStyleSheet(QString("font-size: %1px").arg(20));
    name_line->setStyleSheet(QString("font-size: %1px").arg(20));
    price_lb->setStyleSheet(QString("font-size: %1px").arg(20));
    price_line->setStyleSheet(QString("font-size: %1px").arg(20));
    apply->setStyleSheet(QString("font-size: %1px").arg(25));

    connect(apply, SIGNAL(clicked()), this, SLOT(apply_data()));

    name->addWidget(name_lb);
    name->addWidget(name_line);
    price->addWidget(price_lb);
    price->addWidget(price_line);
    l->addLayout(name, 0, 0);
    l->addLayout(price, 1, 0);
    l->addWidget(apply);
}

add::~add()
{
}

void add::apply_data()
{
    try
    {
        Item res;
        res.data[0] = QString(name_line->text().toStdString().c_str());
        res.data[1] = std::stod(price_line->text().toStdString());
        emit sendData(res);
        close();
    }
    catch (...)
    {
        close();
    }
}


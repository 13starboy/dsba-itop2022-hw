#include "about.h"
#include "ui_about.h"
#include <QPainter>
#include <QLayout>

about::about(QWidget* parent) : QDialog(parent), ui(new Ui::about)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());
    this->setStyleSheet("background-color: rgba(53, 240, 171, 200);");
    ui->label->setStyleSheet("background-color: rgba(255, 255, 255, 0); color: rgba(255, 0, 0, 255); ");
}

about::~about()
{
    delete ui;
}

void about::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setPen({QColorConstants::Svg::black, 9.0});
    painter.setBrush(QBrush(QColorConstants::Svg::orange));
    painter.translate(width() / 2, height() / 2);
    painter.drawLine(5, -155, 110, -265);
    painter.drawLine(5, -155, -85, -255);
    painter.drawLine(-85, -255, 50, -205);
    painter.setPen({QColorConstants::Svg::black, 8.0});
    painter.drawEllipse(-100, -155, 200, 200);
}


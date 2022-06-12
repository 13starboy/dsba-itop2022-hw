#ifndef ADD_H
#define ADD_H

#include <QDialog>
#include <QLineEdit>
#include "mainwindow.h"

class add : public QDialog
{
    Q_OBJECT
    QLineEdit* name_line;
    QLineEdit* price_line;
    QLabel* name_lb;
    QLabel* price_lb;
    QPushButton* apply;

public:
    explicit add(QWidget* parent = nullptr);
    ~add();

signals:
    void sendData(Item& res);

private slots:
    void apply_data();

private:
};

#endif // ADD_H

#include "mainwindow.h"
#include "about.h"
#include "ui_about.h"
#include "cart.h"
#include <QGridLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <fstream>
#include <sstream>
#include <QDebug>
#include <string>
#include <cstdlib>
#include <iomanip>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central_widget = new QWidget (this);
    //cart_model = new CartModel();
    item_model = new ItemModel();
    setCentralWidget(central_widget);
    m_view = new QTableView(central_widget);

    QGridLayout *layout = new QGridLayout(central_widget);
    layout->setRowStretch(0, 1);

    m_textEdit = new QLineEdit();
    find_sum();
    cart_button = new QPushButton();
    add_button = new QPushButton();
    cart_button->setText("Open Cart");
    add_button->setText("Add to Cart");
    connect(cart_button, SIGNAL(clicked()), this, SLOT(open_cart()));
    //connect(add_button, SIGNAL(clicked()), this, SLOT(add_to_cart_function()));
    m_view->setModel(item_model);
    layout->addWidget(cart_button);
    layout->addWidget(add_button);
    layout->addWidget(m_textEdit);
    layout->addWidget (m_view);

    auto file_menu = menuBar ()->addMenu ("File");
    QAction *open_action = new QAction ("Open");
    connect (open_action, &QAction::triggered, this, &MainWindow::load_data_function);
    file_menu->addAction  (open_action);

    QAction *saveas_action = new QAction ("Save As");
    connect (saveas_action, &QAction::triggered, this, &MainWindow::save_data_function);
    file_menu->addAction  (saveas_action);

    QAction *about = new QAction ("About");
    connect (about, &QAction::triggered, this, &MainWindow::about_function);
    menuBar()->addAction(about);

}

void MainWindow::find_sum()
{
    double sum = 0;
    for (auto &item : item_model->m_data)
    {
        sum += item.data[1].toDouble();
    }
    m_textEdit->setText(QString::number(sum, 'f', 2));
}

void MainWindow::open_cart()
{
    cart ct;
    ct.exec();
}

void MainWindow::load_data_function ()
{
    auto dir = QFileDialog::getOpenFileName (this, "Open File", QDir::currentPath ());
    if (!dir.size ())
        return;
    auto data = load_data (dir);
    item_model->set_data (data);
    find_sum();
}

//void MainWindow::add_to_cart_function()
//{
//    auto data = add_to_cart();
//    //cart_model->add_data(data);
//}

//CartItem MainWindow::add_to_cart()
//{
//    CartItem res;
//    QItemSelectionModel *selectModel = m_view->selectionModel();
//    QModelIndexList indexes = selectModel->selectedIndexes();
//    for (QModelIndex ind : indexes)
//    {
//        int row = ind.row();
//        QModelIndex index = m_view->model()->index(row,0,QModelIndex());
//        std::string s = index.data().toString().toStdString();
//        res.data_cart[0] = QString(s.c_str());
//        index = m_view->model()->index(row,1,QModelIndex());
//        s = index.data().toString().toStdString();
//        res.data_cart[1] = QString(s.c_str());
//        res.data_cart[2] = QString::number(1);
//        res.data_cart[3] = res.data_cart[1];
//    }
//    return res;
//}

void MainWindow::save_data_function ()
{
  auto dir = QFileDialog::getSaveFileName (this, "Save File As", QDir::currentPath ());
  if (!dir.size ())
    return;
  save_data (dir);
}

void MainWindow::about_function ()
{
    about ab;
    ab.exec();
}

std::vector<Item> MainWindow::load_data (const QString &dir)
{
    std::fstream file;
    std::vector<Item> result;
    file.open(dir.toStdString ());
    std::string temp1, temp2;
    int j = 0;
    while(std::getline(file, temp1))
    {
        Item res;
        int i = 0;
        bool flag = false;
        std::stringstream str(temp1);
        while(std::getline(str, temp2, ','))
        {
            if(flag == false && i==1)
            {
                flag = true;
                continue;
            }
            if(i==0)
            {
                double price = std::strtod(temp2.c_str(), NULL) / 10;
                res.data[i] = QString::number(price, 'd', 2);
            }
            else
                res.data[i] = QString(temp2.c_str());
            ++i;
        }
        swap(res.data[0], res.data[1]);
        if(j!=0)
            result.push_back(res);
        j++;
    }
    file.close();
    return result;
}

void MainWindow::save_data (const QString &dir)
{
    std::ofstream file (dir.toStdString ());
    int total_columns = static_cast<int> (item_fields::COUNT);
    for (auto &item : item_model->m_data)
    {
        for (int i_column = 0; i_column < total_columns; ++i_column)
        {
            file << item.data[i_column].toString().toStdString();
            if (i_column < total_columns - 1)
                file << ",";
            else
                file << "\n";
        }
    }
}



MainWindow::~MainWindow()
{
}

QVariant ItemModel::data (const QModelIndex &index, int role) const
{
   if (role == Qt::DisplayRole || role == Qt::EditRole)
   {
       const Item &current_item = m_data[index.row ()];
       return current_item.data[index.column ()];
   }
   return {};
}


const char *enum_to_string (item_fields field)
{
    switch (field)
    {
    case item_fields::name     : return "Name";
    case item_fields::price    : return "Price";
    //case item_fields::date     : return "Date";
    case item_fields::COUNT    : return "";
    }
    return {};
}

QVariant ItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        item_fields field = (item_fields)section;
        return enum_to_string (field);
    }
    return {};
}


#include "mainwindow.h"
#include "about.h"
#include "ui_about.h"
#include "cart.h"
#include "add.h"
#include "ui_add.h"
#include <QGridLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <fstream>
#include <sstream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central_widget = new QWidget (this);
    cart_window = new cart;
    item_model = new ItemModel();
    m_view = new QTableView(central_widget);
    setCentralWidget(central_widget);

    QGridLayout *layout = new QGridLayout(central_widget);
    QHBoxLayout *box = new QHBoxLayout(central_widget);

    m_textEdit = new QLabel();
    cart_button = new QPushButton();
    add_to_cart_button = new QPushButton();
    add_item_button = new QPushButton();
    sorting_value = new QComboBox();
    proxyModel = new QSortFilterProxyModel(this);

    m_view->setModel(item_model);
    proxyModel->setSourceModel(item_model);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_textEdit->setStyleSheet(QString("font-size: %1px").arg(25));
    m_textEdit->setText("Total: " + QString::number(0, 'd', 2) + '$');
    cart_button->setText("Open Cart");
    cart_button->setStyleSheet(QString("font-size: %1px").arg(30));
    add_to_cart_button->setText("Add to Cart");
    add_item_button->setText("Add new Item");
    sorting_value->addItem("Default");
    sorting_value->addItem("Price Ascending");
    sorting_value->addItem("Price Descending");
    sorting_value->addItem("Name Ascending");
    sorting_value->addItem("Name Descending");
    add_item_button->setMaximumWidth(200);
    sorting_value->setMaximumWidth(200);
    add_to_cart_button->setMaximumWidth(200);

    connect(cart_button, SIGNAL(clicked()), this, SLOT(open_cart()));
    connect(add_to_cart_button, SIGNAL(clicked()), this, SLOT(add_to_cart_function()));
    connect(sorting_value, SIGNAL(currentIndexChanged(QString)), SLOT(sort_function()));
    connect(add_item_button, SIGNAL(clicked()), this, SLOT(add_item_function()));
    connect(cart_window, SIGNAL(update_cart_sum()), this, SLOT(return_cart_sum()));

    layout->addWidget(cart_button);
    layout->addLayout(box, 1, 0);
    layout->addWidget(sorting_value);
    layout->addWidget(m_textEdit);
    layout->addWidget(m_view);
    box->addWidget(add_item_button);
    box->addWidget(add_to_cart_button);
    box->addStretch();

    auto file_menu = menuBar ()->addMenu ("File");
    QAction *open_action = new QAction ("Open");
    connect (open_action, &QAction::triggered, this, &MainWindow::load_data_function);
    file_menu->addAction(open_action);

    QAction *saveas_action = new QAction ("Save As");
    connect (saveas_action, &QAction::triggered, this, &MainWindow::save_data_function);
    file_menu->addAction(saveas_action);

    QAction *savecart_action = new QAction ("Save cart As");
    connect (savecart_action, &QAction::triggered, this, &MainWindow::save_cart_data_function);
    file_menu->addAction(savecart_action);

    QAction *about = new QAction ("About");
    connect (about, &QAction::triggered, this, &MainWindow::about_function);
    menuBar()->addAction(about);
}

void MainWindow::add_item_function()
{
    add e;
    connect(&e, SIGNAL(sendData(Item&)), this, SLOT(recieveData(Item&)));
    e.exec();
}

void MainWindow::recieveData(Item& res)
{
    item_model->add_data(res);
    proxyModel->setSourceModel(item_model);
}

void MainWindow::sort_function()
{
    QString text = sorting_value->currentText();
    if(text == "Default") {
        m_view->setModel(item_model);
        return;
    }
    else if (text == "Price Ascending")
        proxyModel->sort(1, Qt::AscendingOrder);
    else if (text == "Price Descending")
        proxyModel->sort(1, Qt::DescendingOrder);
    else if (text == "Name Ascending")
        proxyModel->sort(0, Qt::AscendingOrder);
    else if (text == "Name Descending")
        proxyModel->sort(0, Qt::DescendingOrder);   
    m_view->setModel(proxyModel);
}

void MainWindow::return_cart_sum()
{
    long double sum = 0;
    for (auto &item : cart_window->cart_model->m_data_cart)
            sum += item.data_cart[3].toDouble();
    m_textEdit->setText("Total: " + QString::number(sum, 'd', 2) + '$');
    cart_window->c_textEdit->setText("Total: " + QString::number(sum, 'd', 2) + '$');
}

void MainWindow::open_cart()
{
    cart_window->show();

}

void MainWindow::load_data_function ()
{
    auto dir = QFileDialog::getOpenFileName (this, "Open File", QDir::currentPath ());
    if (!dir.size ())
        return;
    auto data = load_data(dir);
    item_model->set_data(data);
}

void MainWindow::add_to_cart_function()
{
    CartItem res;
    QItemSelectionModel *selectModel = m_view->selectionModel();
    QModelIndexList indexes = selectModel->selectedIndexes();
    for (QModelIndex ind : indexes)
    {
        int row = ind.row();
        QModelIndex index = m_view->model()->index(row,0,QModelIndex());
        std::string s = index.data().toString().toStdString();
        res.data_cart[0] = QString(s.c_str());
        index = m_view->model()->index(row,1,QModelIndex());
        res.data_cart[1] = index.data().toDouble();//QString(s.c_str());
        res.data_cart[2] = 1;
        res.data_cart[3] = res.data_cart[1];
    }
    cart_window->cart_model->add_data(res);
    return_cart_sum();
}

void MainWindow::save_data_function ()
{
  auto dir = QFileDialog::getSaveFileName (this, "Save File As", QDir::currentPath ());
  if (!dir.size())
    return;
  save_data(dir);
}

void MainWindow::save_cart_data_function ()
{
  auto dir = QFileDialog::getSaveFileName (this, "Save File As", QDir::currentPath ());
  if (!dir.size())
    return;
  save_cart_data(dir);
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
                res.data[i] = price;
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
    std::ofstream file(dir.toStdString());
    int total_columns = static_cast<int> (item_fields::COUNT);
    for (auto &item : item_model->m_data)
        for (int i_column = 0; i_column < total_columns; ++i_column)
        {
            file << item.data[i_column].toString().toStdString();
            if (i_column < total_columns - 1)
                file << ",";
            else
                file << "\n";
        }
}

void MainWindow::save_cart_data (const QString &dir)
{
    std::ofstream file(dir.toStdString());
    int total_columns = static_cast<int> (cart_fields::COUNT);
    for (auto &item : cart_window->cart_model->m_data_cart)
        for (int i_column = 0; i_column < total_columns; ++i_column)
        {
            file << item.data_cart[i_column].toString().toStdString();
            if (i_column < total_columns - 1)
                file << ",";
            else
                file << "\n";
        }
}

MainWindow::~MainWindow()
{
}

QVariant ItemModel::data (const QModelIndex &index, int role) const
{
   if (role == Qt::DisplayRole || role == Qt::EditRole)
   {
        const Item &current_item = m_data[index.row()];
        if(index.column() == 1)
            return QString::number(current_item.data[index.column()].toDouble(), 'd', 2);
        else
            return current_item.data[index.column ()];
   }
   return {};
}

const char *enum_to_string (item_fields field)
{
    switch (field)
    {
    case item_fields::name     : return "Name";
    case item_fields::price    : return "Price ($)";
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


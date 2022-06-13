#include "mainwindow.h"
#include "about.h"
#include "ui_about.h"
#include "cart.h"
#include "add.h"
#include <QGridLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <fstream>
#include <sstream>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    QWidget* central_widget = new QWidget(this);
    cart_window = new cart;
    item_model = new ItemModel();
    m_view = new QTableView(central_widget);
    setCentralWidget(central_widget);

    QGridLayout* layout = new QGridLayout(central_widget);
    QHBoxLayout* box_menu = new QHBoxLayout();
    QHBoxLayout* box_filter = new QHBoxLayout();

    m_textEdit = new QLabel();
    search_lb = new QLabel();
    cart_button = new QPushButton();
    add_to_cart_button = new QPushButton();
    add_item_button = new QPushButton();
    sorting_value = new QComboBox();
    proxyModel = new FilterModel(this, this);
    search_line = new QLineEdit();
    slide = new QSlider(Qt::Horizontal);
    slide_lb = new QLabel();

    m_view->setModel(proxyModel);
    proxyModel->setSourceModel(item_model);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_textEdit->setStyleSheet(QString("font-size: %1px").arg(25));
    m_textEdit->setText("Total: " + QString::number(0, 'd', 2) + '$');
    cart_button->setText("Open Cart");
    cart_button->setStyleSheet(QString("font-size: %1px").arg(30));
    add_to_cart_button->setText("Add to Cart");
    add_to_cart_button->setStyleSheet(QString("font-size: %1px").arg(20));
    add_to_cart_button->setMaximumWidth(300);
    search_lb->setText("Search:");
    search_lb->setStyleSheet(QString("font-size: %1px").arg(20));
    add_item_button->setText("Add new Item");
    add_item_button->setMaximumWidth(300);
    add_item_button->setStyleSheet(QString("font-size: %1px").arg(20));
    search_line->setStyleSheet(QString("font-size: %1px").arg(20));
    search_line->setMaximumWidth(250);
    sorting_value->addItem("Default");
    sorting_value->addItem("Price Ascending");
    sorting_value->addItem("Price Descending");
    sorting_value->addItem("Name Ascending");
    sorting_value->addItem("Name Descending");
    sorting_value->setStyleSheet(QString("font-size: %1px").arg(18));
    sorting_value->setMaximumWidth(200);
    slide->setRange(0, 500);
    slide->setValue(500);
    slide->setMaximumWidth(250);
    slide_lb->setText("Max Price: 500");
    slide_lb->setStyleSheet(QString("font-size: %1px").arg(20));

    connect(cart_button, SIGNAL(clicked()), this, SLOT(open_cart()));
    connect(add_to_cart_button, SIGNAL(clicked()), this, SLOT(add_to_cart_function()));
    connect(sorting_value, SIGNAL(currentIndexChanged(QString)), SLOT(sort_function()));
    connect(add_item_button, SIGNAL(clicked()), this, SLOT(add_item_function()));
    connect(cart_window, SIGNAL(update_cart_sum()), this, SLOT(return_cart_sum()));
    connect(search_line, SIGNAL(textChanged(QString)), this, SLOT(filter_data()));
    connect(slide, SIGNAL(valueChanged(int)), this, SLOT(update_slide(int)));

    layout->addWidget(cart_button);
    layout->addLayout(box_menu, 1, 0);
    layout->addLayout(box_filter, 2, 0);
    layout->addWidget(m_textEdit);
    layout->addWidget(m_view);
    box_filter->addWidget(sorting_value);
    box_filter->addWidget(search_lb);
    box_filter->addWidget(search_line);
    box_filter->addWidget(slide_lb);
    box_filter->addWidget(slide);
    box_filter->addStretch();
    box_menu->addWidget(add_item_button);
    box_menu->addWidget(add_to_cart_button);
    box_menu->addStretch();

    auto file_menu = menuBar()->addMenu("File");
    QAction* open_action = new QAction("Open");
    connect(open_action, &QAction::triggered, this, &MainWindow::load_data_function);
    file_menu->addAction(open_action);

    QAction* saveas_action = new QAction("Save As");
    connect(saveas_action, &QAction::triggered, this, &MainWindow::save_data_function);
    file_menu->addAction(saveas_action);

    QAction* savecart_action = new QAction("Save cart As");
    connect(savecart_action, &QAction::triggered, this, &MainWindow::save_cart_data_function);
    file_menu->addAction(savecart_action);

    QAction* about = new QAction("About");
    connect(about, &QAction::triggered, this, &MainWindow::about_function);
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
    slide->setRange(0, std::max(max_value, res.data[1].toDouble()));
    max_value = std::max(max_value, res.data[1].toDouble());
}


void MainWindow::update_slide(int position)
{
    float positiond = position / double(1);
    slide_lb->setText("Max Price: " + QString::number(positiond, 'd', 2));
    filter_data();
}

void MainWindow::sort_function()
{
    QString text = sorting_value->currentText();
    if (text == "Default")
        proxyModel->sort(-1, Qt::AscendingOrder);
    else if (text == "Price Ascending")
        proxyModel->sort(1, Qt::AscendingOrder);
    else if (text == "Price Descending")
        proxyModel->sort(1, Qt::DescendingOrder);
    else if (text == "Name Ascending")
        proxyModel->sort(0, Qt::AscendingOrder);
    else if (text == "Name Descending")
        proxyModel->sort(0, Qt::DescendingOrder);
    proxyModel->invalidate();
}

void MainWindow::filter_data()
{
    proxyModel->invalidate();
}

void MainWindow::return_cart_sum()
{
    long double sum = 0;
    for (auto& item : cart_window->cart_model->m_data_cart)
        sum += item.data_cart[3].toDouble();
    m_textEdit->setText("Total: " + QString::number(sum, 'd', 2) + '$');
    cart_window->c_textEdit->setText("Total: " + QString::number(sum, 'd', 2) + '$');
}

void MainWindow::open_cart()
{
    cart_window->show();
}

void MainWindow::load_data_function()
{
    auto dir = QFileDialog::getOpenFileName(this, "Open File", QDir::currentPath());
    if (!dir.size())
        return;
    auto data = load_data(dir);
    item_model->set_data(data);
}

void MainWindow::add_to_cart_function()
{
    CartItem res;
    QItemSelectionModel* selectModel = m_view->selectionModel();
    QModelIndexList indexes = selectModel->selectedIndexes();
    for (QModelIndex ind : indexes)
    {
        int row = ind.row();
        QModelIndex index = m_view->model()->index(row, 0, QModelIndex());
        std::string s = index.data().toString().toStdString();
        res.data_cart[0] = QString(s.c_str());
        index = m_view->model()->index(row, 1, QModelIndex());
        s = index.data().toString().toStdString();
        s.erase(s.end() - 1);
        res.data_cart[1] = QString(s.c_str()).toDouble();
        res.data_cart[2] = 1;
        res.data_cart[3] = res.data_cart[1];
    }
    cart_window->cart_model->add_data(res);
    return_cart_sum();
}

void MainWindow::save_data_function()
{
    auto dir = QFileDialog::getSaveFileName(this, "Save File As", QDir::currentPath());
    if (!dir.size())
        return;
    save_data(dir);
}

void MainWindow::save_cart_data_function()
{
    auto dir = QFileDialog::getSaveFileName(this, "Save File As", QDir::currentPath());
    if (!dir.size())
        return;
    save_cart_data(dir);
}

void MainWindow::about_function()
{
    about ab;
    ab.exec();
}

std::vector<Item> MainWindow::load_data(const QString& dir)
{
    std::fstream file;
    std::vector<Item> result;
    file.open(dir.toStdString());
    std::string temp1, temp2;
    int j = 0;
    while (std::getline(file, temp1))
    {
        Item res;
        int i = 0;
        bool flag = false;
        std::stringstream str(temp1);
        while (std::getline(str, temp2, ','))
        {
            if (flag == false && i == 1)
            {
                flag = true;
                continue;
            }
            if (i == 0)
            {
                double price = std::strtod(temp2.c_str(), NULL) / 10;
                res.data[i] = price;
            }
            else
                res.data[i] = QString(temp2.c_str());
            ++i;
        }
        swap(res.data[0], res.data[1]);
        if (j)
            result.push_back(res);
        j++;
    }
    file.close();
    return result;
}

void MainWindow::save_data(const QString& dir)
{
    std::ofstream file(dir.toStdString());
    int total_columns = static_cast<int> (item_fields::COUNT);
    for (auto& item : item_model->m_data)
        for (int i_column = 0; i_column < total_columns; ++i_column)
        {
            file << item.data[i_column].toString().toStdString();
            if (i_column < total_columns - 1)
                file << ",";
            else
                file << "\n";
        }
}

void MainWindow::save_cart_data(const QString& dir)
{
    std::ofstream file(dir.toStdString());
    int total_columns = static_cast<int> (cart_fields::COUNT);
    for (auto& item : cart_window->cart_model->m_data_cart)
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

FilterModel::FilterModel(QObject* parent, const MainWindow* main_window) : QSortFilterProxyModel(parent), main_window(main_window)
{
}


bool FilterModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{
    if (source_left.column() == 1)
    {
        auto left = source_left.data(Qt::EditRole);
        auto right = source_right.data(Qt::EditRole);
        return left.toDouble() < right.toDouble();
    }
    else
    {
        return QSortFilterProxyModel::lessThan(source_left, source_right);
    }
}


QVariant ItemModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        const Item& current_item = m_data[index.row()];
        if (index.column() == 1)
            return (QString::number(current_item.data[index.column()].toDouble(), 'd', 2) + '$');
        else
            return current_item.data[index.column()];
    }
    else if (role == Qt::EditRole)
    {
        const Item& current_item = m_data[index.row()];
        return current_item.data[index.column()];
    }
    return {};
}

const char* enum_to_string(item_fields field)
{
    switch (field)
    {
    case item_fields::name: return "Name";
    case item_fields::price: return "Price";
    case item_fields::COUNT: return "";
    }
    return {};
}

bool FilterModel::compare(const QString& s, const QString& g) const
{
    std::string a = s.toStdString(), b = g.toStdString();
    std::transform(a.begin(), a.end(), a.begin(), tolower);
    std::transform(b.begin(), b.end(), b.begin(), tolower);
    if (a.size() > b.size())
        return false;
    for (unsigned int i = 0; i < a.size(); i++)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

bool FilterModel::filterAcceptsRow(int source_row, const QModelIndex&/*source_parent*/) const
{
    auto search_model = main_window->search_line->text();
    auto price_model = main_window->slide->value();
    const Item& item = main_window->item_model->m_data[source_row];
    return ((!search_model.size() || compare(search_model, QString(item.data[(int)item_fields::name].toString().toStdString().c_str()))) &&
        price_model >= item.data[(int)item_fields::price].toDouble());
}

QVariant ItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        item_fields field = (item_fields)section;
        return enum_to_string(field);
    }
    return {};
}


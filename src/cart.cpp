#include "cart.h"
#include "mainwindow.h"
#include <QDialog>
#include <QGridLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <fstream>
#include <sstream>
#include <QDebug>

cart::cart(QWidget* parent) : QWidget(parent)
{
    cart_model = new CartModel();
    cart_view = new QTableView(this);

    QGridLayout* l = new QGridLayout(this);
    QHBoxLayout* box_menu = new QHBoxLayout();

    c_textEdit = new QLabel();
    delete_button = new QPushButton();
    remove_button = new QPushButton();
    add_one_button = new QPushButton();
    sort_params = new QComboBox();
    proxy = new FilterProxy(this, this);;

    cart_view->setModel(proxy);
    proxy->setSourceModel(cart_model);
    proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    c_textEdit->setStyleSheet(QString("font-size: %1px").arg(25));
    c_textEdit->setText("Total: " + QString::number(0, 'd', 2) + '$');
    delete_button->setText("Delete Item");
    remove_button->setText("Remove one Item");
    add_one_button->setText("Add one Item");
    delete_button->setStyleSheet(QString("font-size: %1px").arg(20));
    remove_button->setStyleSheet(QString("font-size: %1px").arg(20));
    add_one_button->setStyleSheet(QString("font-size: %1px").arg(20));
    sort_params->addItem("Default");
    sort_params->addItem("Price for one Ascending");
    sort_params->addItem("Price for one Descending");
    sort_params->addItem("Name Ascending");
    sort_params->addItem("Name Descending");
    sort_params->addItem("Total price Ascending");
    sort_params->addItem("Total price Descending");
    sort_params->addItem("Quantity Ascending");
    sort_params->addItem("Quantity Descending");
    sort_params->setStyleSheet(QString("font-size: %1px").arg(18));
    add_one_button->setMaximumWidth(200);
    remove_button->setMaximumWidth(200);
    delete_button->setMaximumWidth(200);
    sort_params->setMaximumWidth(220);

    connect(delete_button, SIGNAL(clicked()), this, SLOT(delete_from_cart_function()));
    connect(remove_button, SIGNAL(clicked()), this, SLOT(remove_one_function()));
    connect(add_one_button, SIGNAL(clicked()), this, SLOT(add_one_function()));
    connect(sort_params, SIGNAL(currentIndexChanged(QString)), SLOT(sort_cart_function()));

    l->addLayout(box_menu, 0, 0);
    box_menu->addWidget(add_one_button);
    box_menu->addWidget(remove_button);
    box_menu->addWidget(delete_button);
    box_menu->addStretch();
    l->addWidget(sort_params);
    l->addWidget(c_textEdit);
    l->addWidget(cart_view);
}

void cart::sort_cart_function()
{
    QString text = sort_params->currentText();
    if (text == "Default")
        proxy->sort(-1, Qt::AscendingOrder);
    else if (text == "Price for one Ascending")
        proxy->sort(1, Qt::AscendingOrder);
    else if (text == "Price for one Descending")
        proxy->sort(1, Qt::DescendingOrder);
    else if (text == "Name Ascending")
        proxy->sort(0, Qt::AscendingOrder);
    else if (text == "Name Descending")
        proxy->sort(0, Qt::DescendingOrder);
    else if (text == "Total price Ascending")
        proxy->sort(3, Qt::AscendingOrder);
    else if (text == "Total price Descending")
        proxy->sort(3, Qt::DescendingOrder);
    else if (text == "Quantity Ascending")
        proxy->sort(2, Qt::AscendingOrder);
    else if (text == "Quantity Descending")
        proxy->sort(2, Qt::DescendingOrder);
    proxy->invalidate();
}

CartItem cart::collect_data()
{
    CartItem res;
    QItemSelectionModel* selectModel = cart_view->selectionModel();
    QModelIndexList indexes = selectModel->selectedIndexes();
    for (QModelIndex ind : indexes)
    {
        int row = ind.row();
        QModelIndex index = cart_view->model()->index(row, 0, QModelIndex());
        std::string s = index.data().toString().toStdString();
        res.data_cart[0] = s.c_str();
        index = cart_view->model()->index(row, 1, QModelIndex());
        s = index.data().toString().toStdString();
        s.erase(s.end() - 1);
        res.data_cart[1] = QString(s.c_str()).toDouble();
        index = cart_view->model()->index(row, 2, QModelIndex());
        s = index.data().toString().toStdString();
        res.data_cart[2] = QString(s.c_str()).toDouble();
        index = cart_view->model()->index(row, 3, QModelIndex());
        s = index.data().toString().toStdString();
        s.erase(s.end() - 1);
        res.data_cart[3] = QString(s.c_str()).toDouble();
        break;
    }
    return res;
}

void cart::delete_from_cart_function()
{
    CartItem res = collect_data();
    cart_model->delete_data(res);
    proxy->setSourceModel(cart_model);
    emit update_cart_sum();
}

void cart::remove_one_function()
{
    CartItem res = collect_data();
    cart_model->remove_one(res);
    proxy->setSourceModel(cart_model);
    emit update_cart_sum();
}

void cart::add_one_function()
{
    CartItem res = collect_data();
    cart_model->add_data(res);
    proxy->setSourceModel(cart_model);
    emit update_cart_sum();
}

cart::~cart()
{
}

FilterProxy::FilterProxy(QObject* parent, const cart* cart_window) : QSortFilterProxyModel(parent), cart_window(cart_window)
{
}

bool FilterProxy::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{
    if (source_left.column() == 1 || source_left.column() == 3)
    {
        auto left = source_left.data(Qt::EditRole);
        auto right = source_right.data(Qt::EditRole);
        return left.toDouble() < right.toDouble();
    }
    else
        return QSortFilterProxyModel::lessThan(source_left, source_right);
}

QVariant CartModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        const CartItem& current_item = m_data_cart[index.row()];
        if (index.column() == 1 || index.column() == 3)
            return (QString::number(current_item.data_cart[index.column()].toDouble(), 'd', 2) + '$');
        else
            return current_item.data_cart[index.column()];
    }
    else if (role == Qt::EditRole)
    {
        const CartItem& current_item = m_data_cart[index.row()];
        return current_item.data_cart[index.column()];
    }
    return {};
}


const char* enum_to_string_cart(cart_fields field)
{
    switch (field)
    {
    case cart_fields::name: return "Name";
    case cart_fields::price: return "Price for one";
    case cart_fields::quantity: return "Quantity";
    case cart_fields::total: return "Total cost";
    case cart_fields::COUNT: return "";
    }
    return {};
}

QVariant CartModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        cart_fields field = (cart_fields)section;
        return enum_to_string_cart(field);
    }
    return {};
}


#include "cart.h"
#include "mainwindow.h"
#include <QDialog>
#include <QGridLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <fstream>
#include <sstream>
#include <QDebug>

cart::cart(QWidget *parent) :
    QWidget(parent)
{
    cart_model = new CartModel();
    cart_view = new QTableView(this);
    QGridLayout *l = new QGridLayout(this);

    cart_view->setModel(cart_model);

    l->addWidget(cart_view);
}

cart::~cart()
{
}

QVariant CartModel::data (const QModelIndex &index, int role) const
{
   if (role == Qt::DisplayRole || role == Qt::EditRole)
   {
       const CartItem &current_cart_item = m_data_cart[index.row ()];
       return current_cart_item.data_cart[index.column ()];
   }
   return {};
}


const char *enum_to_string_cart (cart_fields field)
{
    switch (field)
    {
    case cart_fields::name     : return "Name";
    case cart_fields::price    : return "Price for one ($)";
    case cart_fields::quantity : return "Quantity";
    case cart_fields::total    : return "Total cost ($)";
    case cart_fields::COUNT    : return "";
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

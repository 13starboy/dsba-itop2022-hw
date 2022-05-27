#ifndef CART_H
#define CART_H

#include <QMainWindow>
#include <QDialog>
#include <QTableView>

class CartModel;
class CartItem;

class cart : public QDialog
{
    Q_OBJECT
    QTableView *cart_view = nullptr;
    CartModel *cart_model = nullptr;

public:
    explicit cart(QWidget *parent = nullptr);
    ~cart();

signals:


public slots:


};

enum class cart_fields
{
    name,
    price,
    count,
    total,
    COUNT
};

const char *enum_to_string_cart (cart_fields field);

class CartItem
{
public:
    std::array<QVariant, (static_cast<int> (cart_fields::COUNT))> data_cart;
};

class CartModel : public QAbstractTableModel
{
Q_OBJECT

public:
    std::vector<CartItem> m_data_cart;

int rowCount(const QModelIndex &/*parent*/) const override
{
    return m_data_cart.size ();
}

int columnCount(const QModelIndex &/*parent*/) const override
{
    return static_cast<int> (cart_fields::COUNT);
}

void set_data (const std::vector<CartItem> &data_cart)
{
  beginResetModel ();
  m_data_cart = data_cart; // dangerous
  endResetModel ();
  emit layoutChanged ();
}

void add_data(const CartItem &data)
{
    beginResetModel ();
    m_data_cart.push_back(data); // dangerous
    endResetModel ();
    emit layoutChanged ();
}

QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const override;
QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
#endif // CART_H

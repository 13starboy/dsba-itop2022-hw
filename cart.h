#ifndef CART_H
#define CART_H

#include <QMainWindow>
#include <QDialog>
#include <QTableView>

class CartModel;
class CartItem;

class cart : public QWidget
{
    Q_OBJECT
    QTableView *cart_view = nullptr;
    public:
    CartModel *cart_model = nullptr;//      private???

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
    quantity,
    total,
    COUNT
};

const char *enum_to_string_cart (cart_fields field);

class CartItem
{
public:
    std::array<QVariant, (static_cast<int> (cart_fields::COUNT))> data_cart;
    friend bool operator==(const CartItem &a, const CartItem &b) {
        return(a.data_cart[0] == b.data_cart[0] && a.data_cart[1] == b.data_cart[1]);
    }
    friend bool operator!=(const CartItem &a, const CartItem &b) {
        return(a.data_cart[0] != b.data_cart[0] || a.data_cart[1] != b.data_cart[1]);
    }
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

void add_data(const CartItem &data)
{
    bool flag = true;
    beginResetModel ();
    for(auto &item : m_data_cart) {
        if(item == data) {
            item.data_cart[2] = QString::number(item.data_cart[2].toDouble() + 1);
            item.data_cart[3] = QString::number(item.data_cart[3].toDouble() + item.data_cart[1].toDouble(), 'd', 2);
            flag = false;
            break;
        }
    }
    if(flag)
        m_data_cart.push_back(data);
    endResetModel ();
    emit layoutChanged ();
}

QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const override;
QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
#endif // CART_H

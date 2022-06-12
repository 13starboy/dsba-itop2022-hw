#ifndef CART_H
#define CART_H

#include <QMainWindow>
#include <QDialog>
#include <QTableView>
#include <QLineEdit>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QComboBox>
#include <QLabel>

class CartModel;
class CartItem;
class FilterProxy;

class cart : public QWidget
{
    Q_OBJECT
    QTableView* cart_view = nullptr;
    QPushButton* delete_button;
    QPushButton* remove_button;
    QPushButton* add_one_button;
    QSortFilterProxyModel* proxy;
    QComboBox* sort_params;

public:
    CartModel* cart_model = nullptr;
    QLabel* c_textEdit;
    explicit cart(QWidget* parent = nullptr);
    ~cart();

private:
    CartItem collect_data();

signals:
    void update_cart_sum();

private slots:
    void delete_from_cart_function();
    void add_one_function();
    void remove_one_function();
    void sort_cart_function();

    friend class FilterProxy;
};

enum class cart_fields
{
    name,
    price,
    quantity,
    total,
    COUNT
};

const char* enum_to_string_cart(cart_fields field);

class CartItem
{
public:
    std::array<QVariant, (static_cast<int> (cart_fields::COUNT))> data_cart;
    friend bool operator==(const CartItem& a, const CartItem& b)
    {
        return(a.data_cart[0] == b.data_cart[0] && a.data_cart[1] == b.data_cart[1]);
    }
    friend bool operator!=(const CartItem& a, const CartItem& b)
    {
        return(a.data_cart[0] != b.data_cart[0] || a.data_cart[1] != b.data_cart[1]);
    }
};

class FilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    FilterProxy(QObject* parent, const cart* cart_window);
    bool compare(const QString& s, const QString& g) const;
protected:
    bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;
private:
    const cart* cart_window = nullptr;
};

class CartModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    std::vector<CartItem> m_data_cart;
    int rowCount(const QModelIndex&/*parent*/) const override
    {
        return m_data_cart.size();
    }

    int columnCount(const QModelIndex&/*parent*/) const override
    {
        return static_cast<int> (cart_fields::COUNT);
    }

    void add_data(const CartItem& data)
    {
        if (data.data_cart[0].isNull() || data.data_cart[1].isNull())
            return;
        bool flag = true;
        beginResetModel();
        for (auto& item : m_data_cart)
        {
            if (item == data)
            {
                item.data_cart[2] = item.data_cart[2].toDouble() + 1;
                item.data_cart[3] = item.data_cart[3].toDouble() + item.data_cart[1].toDouble();
                flag = false;
                break;
            }
        }
        if (flag)
            m_data_cart.push_back(data);
        endResetModel();
        emit layoutChanged();
    }

    void delete_data(const CartItem& data)
    {
        if (data.data_cart[0].isNull() || data.data_cart[1].isNull())
            return;
        beginResetModel();
        std::vector<CartItem>::iterator it = m_data_cart.begin();
        for (; it != m_data_cart.end(); it++)
        {
            if (*it == data)
            {
                m_data_cart.erase(it);
                break;
            }
        }
        endResetModel();
        emit layoutChanged();
    }

    void remove_one(const CartItem& data)
    {
        if (data.data_cart[0].isNull() || data.data_cart[1].isNull())
            return;
        beginResetModel();
        std::vector<CartItem>::iterator it = m_data_cart.begin();
        for (; it != m_data_cart.end(); it++)
        {
            if (*it == data)
            {
                if (data.data_cart[2].toInt() > 1)
                {
                    it->data_cart[2] = it->data_cart[2].toDouble() - 1;
                    it->data_cart[3] = it->data_cart[3].toDouble() - it->data_cart[1].toDouble();
                }
                else
                {
                    m_data_cart.erase(it);
                    break;
                }
            }
        }
        endResetModel();
        emit layoutChanged();
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};
#endif // CART_H

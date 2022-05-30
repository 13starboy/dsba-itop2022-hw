#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include "cart.h"
#include <QSortFilterProxyModel>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <algorithm>

class ItemModel;
class Item;

class MainWindow : public QMainWindow
{
public:
    Q_OBJECT
    QTableView *m_view = nullptr;
    ItemModel *item_model = nullptr;
    QLabel *m_textEdit;
    QPushButton *cart_button;
    QPushButton *add_to_cart_button;
    QPushButton *add_item_button;
    QComboBox *sorting_value;
    QSortFilterProxyModel *proxyModel;

public:
    MainWindow(QWidget *parent = nullptr);
    std::vector<Item> load_data (const QString &dir);
    void save_data (const QString &dir);
    void save_cart_data (const QString &dir);
    cart *cart_window;
    ~MainWindow();

public slots:
    void return_cart_sum();
    void recieveData(Item& res);

private slots:
    void load_data_function();
    void save_data_function();
    void save_cart_data_function();
    void about_function();
    void open_cart();
    void add_to_cart_function();
    void sort_function();
    void add_item_function();
};

enum class item_fields
{
    name,
    price,
    COUNT
};

const char *enum_to_string (item_fields field);

class Item
{
public:
    std::array<QVariant, (static_cast<int> (item_fields::COUNT))> data;
};

class ItemModel : public QAbstractTableModel
{
Q_OBJECT

public:
std::vector<Item> m_data;
int rowCount(const QModelIndex &/*parent*/) const override
{
    return m_data.size ();
}

int columnCount(const QModelIndex &/*parent*/) const override
{
    return static_cast<int> (item_fields::COUNT);
}

void set_data (const std::vector<Item> &data)
{
    beginResetModel ();
    m_data.insert(m_data.begin(), data.begin(), data.end());
    endResetModel ();
    emit layoutChanged ();
}

void add_data (const Item &data)
{
    if(data.data[0].isNull() || data.data[1].isNull())
        return;
    beginResetModel ();
    m_data.insert(m_data.begin(), data);
    endResetModel ();
    emit layoutChanged ();
}

QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const override;
QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

#endif // MAINWINDOW_H

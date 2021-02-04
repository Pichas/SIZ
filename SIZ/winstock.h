#ifndef WINSTOCK_H
#define WINSTOCK_H

#include <QDialog>
#include <QtSql>
#include <QScopedPointer>
#include <QDebug>
#include <QMessageBox>
#include <QAxObject>
#include <QFileDialog>
#include <QSortFilterProxyModel>

#include "winnames.h"

namespace Ui {
class winStock;
}

class winStock : public QDialog
{
    Q_OBJECT

public:
    explicit winStock(QWidget *parent = nullptr);
    ~winStock();

private:
    Ui::winStock *ui;

    QScopedPointer<QSqlTableModel> namesModel;
    QScopedPointer<QSqlQueryModel> stockModel;
    QScopedPointer<QSortFilterProxyModel> stockModelFiltered;

    QScopedPointer<QSortFilterProxyModel> selectNameSortedModel;
    QScopedPointer<QSqlTableModel> sizeModel;
    QScopedPointer<QSqlQueryModel> clothesModel;


private slots:
    void refreshNamesAndSizes(QString newVal);
    void loadCount();
    void setNewVal();
    void exportTable();
};

#endif // WINSTOCK_H

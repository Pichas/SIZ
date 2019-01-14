#ifndef WINSETPRICE_H
#define WINSETPRICE_H

#include <QDialog>
#include <QSqlTableModel>
#include <QDebug>
#include <QSqlQuery>

namespace Ui {
class winSetPrice;
}

class winSetPrice : public QDialog
{
    Q_OBJECT

public:
    explicit winSetPrice(QString IDnaim, QWidget *parent = nullptr);
    ~winSetPrice();

private:
    Ui::winSetPrice *ui;
    QScopedPointer<QSqlTableModel> mainTableModel;
};

#endif // WINSETPRICE_H

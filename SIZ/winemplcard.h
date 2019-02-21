#ifndef WINEMPLCARD_H
#define WINEMPLCARD_H

#include <QDialog>
#include <QAxObject>
#include <QtSql>
#include <QTableView>
#include <QHeaderView>
#include <QMessageBox>

#include "myset.h"

namespace Ui {
class winEmplCard;
}

class winEmplCard : public QDialog
{
    Q_OBJECT

public:
    explicit winEmplCard(QWidget *parent = nullptr);
    ~winEmplCard() override;


    virtual void accept() override;

private:
    Ui::winEmplCard *ui;
    QScopedPointer<QSqlQueryModel> emplListModel;

};

#endif // WINEMPLCARD_H

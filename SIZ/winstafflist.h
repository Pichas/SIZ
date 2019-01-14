#ifndef WINSTAFFLIST_H
#define WINSTAFFLIST_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>

#include "modelstafflist.h"
#include "wintables.h"

namespace Ui {
class winStaffList;
}

class winStaffList : public QDialog
{
    Q_OBJECT

public:
    explicit winStaffList(QWidget *parent = nullptr);
    ~winStaffList();

private:
    Ui::winStaffList *ui;
    QScopedPointer<modelStaffList> mainTableModel;
};

#endif // WINSTAFFLIST_H

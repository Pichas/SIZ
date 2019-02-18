#ifndef WINDBLIST_H
#define WINDBLIST_H

#include <QDialog>
#include <QDebug>
#include <QInputDialog>
#include <QFileDialog>



#include "myset.h"

namespace Ui {
class winDBList;
}

class winDBList : public QDialog
{
    Q_OBJECT

public:
    explicit winDBList(QWidget *parent = nullptr);
    ~winDBList();

private:
    Ui::winDBList *ui;
    void insertNewInfoInTable(QModelIndex x);
    void saveDBList(bool b);
};

#endif // WINDBLIST_H

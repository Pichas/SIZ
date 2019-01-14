#ifndef WINNAMES_H
#define WINNAMES_H

#include <QDialog>
#include <QScopedPointer>
#include <QSqlTableModel>
#include <QAction>
#include <QMessageBox>

#include <modelnames.h>
#include <winsetprice.h>

namespace Ui {
class winNames;
}

class winNames : public QDialog
{
    Q_OBJECT

public:
    explicit winNames(QWidget *parent = nullptr);
    ~winNames();

private:
    Ui::winNames *ui;
    QScopedPointer<modelNames> mainTableModel;

    void setPrice(bool b);
};

#endif // WINNAMES_H

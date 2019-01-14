#ifndef WINTABLES_H
#define WINTABLES_H

#include <QDialog>
#include <QSqlTableModel>
#include <QScopedPointer>
#include <QDebug>
#include <QMessageBox>


namespace Ui {
class winTables;
}

class winTables : public QDialog
{
    Q_OBJECT

public:
    explicit winTables(QStringList table, QWidget *parent = nullptr);
    ~winTables();

private:
    Ui::winTables *ui;
    QScopedPointer<QSqlTableModel> mainTableModel;
};

#endif // WINTABLES_H

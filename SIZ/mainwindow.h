#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QTableView>
#include <QScopedPointer>
#include <QMessageBox>







#include "update/appupdate.h"
#include "report/mytable.h"
#include "winstafflist.h"
#include "winnames.h"
#include "winnorma.h"
#include "winstock.h"
#include "winreport.h"
#include "winemplcard.h"
#include "windblist.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;


private slots:
    void checkUpdate();


};

#endif // MAINWINDOW_H

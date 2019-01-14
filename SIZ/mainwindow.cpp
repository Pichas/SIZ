#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "report/mytable.h"
#include "winstafflist.h"
#include "winnames.h"
#include "winnorma.h"
#include "winstock.h"
#include "winreport.h"
#include "winemplcard.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pbStaffList, &QPushButton::clicked, [&](bool /*b*/){
        QScopedPointer <winStaffList> win(new winStaffList(nullptr));
        win->exec();});

    connect(ui->pbNames, &QPushButton::clicked, [&](bool /*b*/){
        QScopedPointer <winNames> win(new winNames(nullptr));
        win->exec();});

    connect(ui->pbNorm, &QPushButton::clicked, [&](bool /*b*/){
        QScopedPointer <winNorma> win(new winNorma(nullptr));
        win->exec();});

    connect(ui->pbStock, &QPushButton::clicked, [&](bool /*b*/){
        QScopedPointer <winStock> win(new winStock(nullptr));
        win->exec();});

    connect(ui->pbEmplCard, &QPushButton::clicked, [&](bool /*b*/){
        QScopedPointer <winEmplCard> win(new winEmplCard(nullptr));
        win->exec();});

    connect(ui->pbReport, &QPushButton::clicked, [&](bool /*b*/){
        QScopedPointer <winReport> win(new winReport(nullptr));
        win->exec();});

}

MainWindow::~MainWindow()
{
    delete ui;
}

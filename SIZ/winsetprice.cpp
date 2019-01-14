#include "winsetprice.h"
#include "ui_winsetprice.h"

winSetPrice::winSetPrice(QString IDnaim, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::winSetPrice)
{
    ui->setupUi(this);

    mainTableModel.reset(new QSqlTableModel(this, QSqlDatabase::database("mainBase")));
    mainTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    mainTableModel->setTable("цена");
    mainTableModel->setFilter("ИДнаим = '" + IDnaim + "'");
    mainTableModel->select();

    ui->tvTable->setModel(mainTableModel.data());

    //спрятать лишние столбцы
    ui->tvTable->hideColumn(0);
    ui->tvTable->hideColumn(1);

    ui->tvTable->resizeColumnsToContents();
    ui->tvTable->resizeRowsToContents();
    ui->tvTable->sortByColumn(2, Qt::AscendingOrder);


    connect(ui->pbAdd,      &QPushButton::clicked, [&](bool /*b*/){
        mainTableModel->insertRow(0);
        mainTableModel->setData(mainTableModel->index(0,1), IDnaim);}); //добавляемая запись относится к текущему наименованию

    connect(ui->pbDel,      &QPushButton::clicked, [&](bool /*b*/){
        if (!ui->tvTable->selectionModel()->hasSelection()) return;
        mainTableModel->removeRow(ui->tvTable->selectionModel()->selectedRows().at(0).row());});

    connect(ui->pbSubmit,   &QPushButton::clicked, [&](bool /*b*/){mainTableModel->submitAll();});
    connect(ui->pbRevert,   &QPushButton::clicked, [&](bool /*b*/){mainTableModel->revertAll();});
}

winSetPrice::~winSetPrice()
{
    delete ui;
}

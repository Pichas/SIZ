#include "wintables.h"
#include "ui_wintables.h"

winTables::winTables(QStringList table, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::winTables)
{
    ui->setupUi(this);
    ui->cbTableName->addItems(table);

    mainTableModel.reset(new QSqlTableModel(this, QSqlDatabase::database("mainBase")));
    mainTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    ui->tvTable->setModel(mainTableModel.data());

    connect(ui->cbTableName, &QComboBox::currentTextChanged, [&](QString newValue){
        mainTableModel->setTable(newValue);
        mainTableModel->select();
        ui->tvTable->resizeColumnsToContents();
        ui->tvTable->resizeRowsToContents();
        ui->tvTable->sortByColumn(0, Qt::AscendingOrder);
    });
    ui->cbTableName->currentTextChanged(table.at(0));

    connect(ui->pbAdd,      &QPushButton::clicked, [&](bool /*b*/){mainTableModel->insertRow(0);});
    connect(ui->pbDel,      &QPushButton::clicked, [&](bool /*b*/){
        if (!ui->tvTable->selectionModel()->hasSelection()) return;
        QMessageBox::information(nullptr, "Информация", "При удалении записи, также удалятся все данные об этой записи из других таблиц.");
        mainTableModel->removeRow(ui->tvTable->selectionModel()->selectedRows().at(0).row());});

    connect(ui->pbSubmit,   &QPushButton::clicked, [&](bool /*b*/){mainTableModel->submitAll();});
    connect(ui->pbRevert,   &QPushButton::clicked, [&](bool /*b*/){mainTableModel->revertAll();});
    connect(ui->pbClose,    &QPushButton::clicked, this, &QDialog::accept);
}

winTables::~winTables()
{
    delete ui;
}

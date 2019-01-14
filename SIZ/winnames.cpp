#include "winnames.h"
#include "ui_winnames.h"

winNames::winNames(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::winNames)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);

    mainTableModel.reset(new modelNames(this, QSqlDatabase::database("mainBase")));
    mainTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    mainTableModel->setTable("наименования");
    mainTableModel->select();

    ui->tvTable->setModel(mainTableModel.data());
    ui->tvTable->setItemDelegate(new delegateNames(this));

    //окно с ценами
    QAction* setPriceAction = new QAction("Указать цены", this);
    connect(setPriceAction, &QAction::triggered, this, &winNames::setPrice);
    connect(ui->pbSetPrice, &QPushButton::clicked, this, &winNames::setPrice);
    ui->tvTable->addAction(setPriceAction);

    connect(ui->pbAdd,      &QPushButton::clicked, [&](bool /*b*/){mainTableModel->insertRow(0);});
    connect(ui->pbDel,      &QPushButton::clicked, [&](bool /*b*/){
        if (!ui->tvTable->selectionModel()->hasSelection()) return;
        QMessageBox::information(nullptr, "Информация", "При удалении записи, также удалятся все данные об этой записи из других таблиц.");
        mainTableModel->removeRow(ui->tvTable->selectionModel()->selectedRows().at(0).row());});

    connect(ui->pbSubmit,   &QPushButton::clicked, [&](bool /*b*/){mainTableModel->submitAll();});
    connect(ui->pbRevert,   &QPushButton::clicked, [&](bool /*b*/){mainTableModel->revertAll();});

    connect(ui->pbClose,    &QPushButton::clicked, this, &QDialog::accept);



    mainTableModel->setHeaderData(1, Qt::Horizontal, "Наименования"); //в базе столбец называется "наимен"

    ui->tvTable->resizeColumnsToContents();
    ui->tvTable->resizeRowsToContents();
}

winNames::~winNames()
{
    delete ui;
}

void winNames::setPrice(bool /*b*/)
{
    if (!ui->tvTable->selectionModel()->hasSelection()) return;

    QString IDNaim = ui->tvTable->selectionModel()->selectedRows().at(0).data().toString();
    QScopedPointer<winSetPrice> win(new winSetPrice(IDNaim, nullptr));
    win->exec();
}

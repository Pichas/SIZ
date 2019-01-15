#include "winstafflist.h"
#include "ui_winstafflist.h"

winStaffList::winStaffList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::winStaffList)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);

    mainTableModel.reset(new modelStaffList(this, QSqlDatabase::database("mainBase")));
    mainTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    mainTableModel->setTable("работники");
    mainTableModel->select();

    ui->tvTable->setModel(mainTableModel.data());
    ui->tvTable->setItemDelegate(new delegateStaffList(this));

    connect(ui->pbAdd,      &QPushButton::clicked, [&](bool /*b*/){mainTableModel->insertRow(0);});
    connect(ui->pbDel,      &QPushButton::clicked, [&](bool /*b*/){
        if (!ui->tvTable->selectionModel()->hasSelection()) return;
        QMessageBox::information(nullptr, "Информация", "При удалении работника, также удалятся все данные о нем из других таблиц.");
        mainTableModel->removeRow(ui->tvTable->selectionModel()->selectedRows().at(0).row());});

    connect(ui->pbSubmit,   &QPushButton::clicked, [&](bool /*b*/){mainTableModel->submitAll();});
    connect(ui->pbRevert,   &QPushButton::clicked, [&](bool /*b*/){mainTableModel->revertAll();});

    connect(ui->pbClose,    &QPushButton::clicked, this, &QDialog::accept);

    //окно с доп таблицами
    connect(ui->pbTables,   &QPushButton::clicked, [&](bool /*b*/){
        QDialog::accept(); //закрыть текущее окно
        QStringList tables {"цех", "должности"};
        QScopedPointer<winTables> win(new winTables(tables, nullptr));
        win->exec();});




    mainTableModel->setHeaderData(6, Qt::Horizontal, "Дата устр"); //в базе столбец называется ДатаПоступления
    mainTableModel->setHeaderData(7, Qt::Horizontal, "Дата перев"); //в базе столбец называется ДатаПеревода
    mainTableModel->setHeaderData(10, Qt::Horizontal, "Одежда"); //в базе столбец называется РазмерОдежды
    mainTableModel->setHeaderData(11, Qt::Horizontal, "Обувь"); //в базе столбец называется РазмерОбуви
    mainTableModel->setHeaderData(12, Qt::Horizontal, "Голова"); //в базе столбец называется РазмерГоловногоУбора
    mainTableModel->setHeaderData(13, Qt::Horizontal, "Противогаз"); //в базе столбец называется РазмерГоловногоУбора
    mainTableModel->setHeaderData(14, Qt::Horizontal, "Респир"); //в базе столбец называется РазмерРеспиратора
    mainTableModel->setHeaderData(15, Qt::Horizontal, "Рукавицы"); //в базе столбец называется РазмерРукавиц
    mainTableModel->setHeaderData(16, Qt::Horizontal, "Перчатки"); //в базе столбец называется РазмерПерчаток
    mainTableModel->setHeaderData(17, Qt::Horizontal, "Каска"); //в базе столбец называется ЦветКаски

    ui->tvTable->resizeColumnsToContents();
    ui->tvTable->resizeRowsToContents();
}

winStaffList::~winStaffList()
{
    delete ui;
}

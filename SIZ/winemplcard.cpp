#include "winemplcard.h"
#include "ui_winemplcard.h"

winEmplCard::winEmplCard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::winEmplCard)
{
    ui->setupUi(this);

    connect(ui->pbLoad, &QPushButton::clicked, this, &winEmplCard::accept);
    connect(ui->pbCancel, &QPushButton::clicked, this, &winEmplCard::reject);


    emplListModel.reset(new QSqlQueryModel(this));
    emplListModel->setQuery(" SELECT ТабНомер, Фамилия, Имя, Отчество "
                       " FROM работники "
                       " ORDER BY работники.Фамилия;", QSqlDatabase::database("mainBase"));

    QTableView* emplListView = new QTableView(this);
    ui->cbEmplList->setModel(emplListModel.data());

    emplListView->setModel(emplListModel.data());
    emplListView->horizontalHeader()->setVisible(false);
    emplListView->verticalHeader()->setVisible(false);
    emplListView->resizeColumnsToContents();
    emplListView->resizeRowsToContents();
    emplListView->setSelectionMode(QAbstractItemView::SingleSelection);
    emplListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->cbEmplList->setView(emplListView);
}

winEmplCard::~winEmplCard()
{
    delete ui;
}

void winEmplCard::accept()
{
    if (ui->cbEmplList->findText(ui->cbEmplList->currentText()) == -1) {
        QMessageBox::warning(nullptr, "Ошибка", "Такого работника не существует");
        return;
    }



    QAxObject* excel = new QAxObject("Excel.Application");
    QAxObject* app = excel->querySubObject("Application()");
    QAxObject* wbks = excel->querySubObject("Workbooks()");
    QAxObject* wb = wbks->querySubObject("Open(const QString&)", QCoreApplication::applicationDirPath() + "/tmpl/emplCard.xltm" );

    QAxObject* ws = wb->querySubObject("Worksheets(int)", 3);
    QAxObject* Cell1 = ws->querySubObject("Cells(QVariant&,QVariant&)", 1, 1);
    QAxObject* Cell2 = ws->querySubObject("Cells(QVariant&,QVariant&)", 2, 1);

    Cell1->setProperty("Value", INI->value("dbFile", "").toString()); //записать путь к базе для работы
    Cell2->setProperty("Value", ui->cbEmplList->currentText()); //указать таб номер работника

    //показать excel
    app->setProperty("Visible", true );

    excel->dynamicCall("Run (QVariant)", "loadNames");

    excel->deleteLater();
    QDialog::accept();
}

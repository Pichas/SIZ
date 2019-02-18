#include "windblist.h"
#include "ui_windblist.h"

winDBList::winDBList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::winDBList)
{
    ui->setupUi(this);

    connect(ui->pbAdd, &QPushButton::clicked, [&](bool /*b*/){
        ui->twDBList->insertRow(0);
        ui->twDBList->setItem(0,0, new QTableWidgetItem(""));
        ui->twDBList->setItem(0,1, new QTableWidgetItem(""));
    });
    connect(ui->pbDel, &QPushButton::clicked, [&](bool /*b*/){ui->twDBList->removeRow(ui->twDBList->selectedItems().at(0)->row());});
    connect(ui->pbClose, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->pbSaveDBList, &QPushButton::clicked,  this, &winDBList::saveDBList);
    connect(ui->twDBList, &QTableWidget::doubleClicked, this, &winDBList::insertNewInfoInTable);

    //загруить данные
    for (int i = 0; i < INI->allKeys().count(); i++) {
        if (INI->allKeys().at(i)[0] != '_') continue; //если не начинается с нижнего прочерка, то следующий
        ui->twDBList->insertRow(i);
        ui->twDBList->setItem(i, 0, new QTableWidgetItem(INI->allKeys().at(i).mid(1))); //удалить начало ключа _
        ui->twDBList->setItem(i, 1, new QTableWidgetItem(INI->value(INI->allKeys().at(i), " ").toString()));
    }
}

winDBList::~winDBList()
{
    delete ui;
}

void winDBList::insertNewInfoInTable(QModelIndex x)
{
    if (x.column() == 0){
        bool ok;
        QString text = QInputDialog::getText(this, "Название филиала",
                                             tr("Название филиала:"), QLineEdit::Normal,
                                             x.data().toString(), &ok);
        if (ok && !text.isEmpty())
            ui->twDBList->setItem(x.row(), x.column(), new QTableWidgetItem(text));
    }

    if (x.column() == 1){
        QString pathToDB = QFileDialog::getOpenFileName(nullptr,
                                                        "Открыть файл базы данных",
                                                        QDir(x.data().toString()).path(),
                                                        "Microsoft Access (*.mdb, *.accdb)");
        if (!pathToDB.isEmpty())
            ui->twDBList->setItem(x.row(), x.column(), new QTableWidgetItem(pathToDB));
    }
}

void winDBList::saveDBList(bool /*b*/)
{
    for (int i = 0; i < INI->allKeys().count(); i++) { //сначало удалить все записи по станциям
        if (INI->allKeys().at(i)[0] == '_')
            INI->remove(INI->allKeys().at(i));
    }
    for (int i = 0; i < ui->twDBList->rowCount(); i++) { //добавить запси по станциям
        INI->setValue("_" + ui->twDBList->item(i, 0)->data(Qt::DisplayRole).toString(), //ключ начинается с _
                      ui->twDBList->item(i, 1)->data(Qt::DisplayRole).toString()); //значение
    }
    INI->sync(); //дождатся запси в файл
}

#include "winstock.h"
#include "ui_winstock.h"

winStock::winStock(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::winStock)
{
    ui->setupUi(this);

    //модель с наименованиями
    namesModel.reset(new QSqlTableModel(this, QSqlDatabase::database("mainBase")));
    namesModel->setTable("наименования");
    namesModel->select();

    ui->cbID->setModel(namesModel.data());
    ui->cbName->setModel(namesModel.data());
    ui->cbName->setModelColumn(1); //отобразить содержимое первого столбца (название)


    //если расширить окно, то станут доступны остальные поля с информацией по наименованиям
    QTableView* namesView = new QTableView(this);
    namesView->setModel(namesModel.data());
    namesView->sortByColumn(1, Qt::AscendingOrder);
    namesView->hideColumn(0);
    namesView->horizontalHeader()->setVisible(false);
    namesView->verticalHeader()->setVisible(false);
    namesView->resizeColumnsToContents();
    namesView->setColumnWidth(1, 679 - qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent)); //размер столбца под текущий размер окна минус полоса прокрутки
    namesView->resizeRowsToContents();
    namesView->setSelectionMode(QAbstractItemView::SingleSelection);
    namesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->cbName->setView(namesView);


    //модель для сортировки и выделения нужной записи из таблицы с наименованиями
    selectNameSortedModel.reset(new QSortFilterProxyModel(this));
    selectNameSortedModel->setSourceModel(namesModel.data());
    sizeModel.reset(new QSqlTableModel(this, QSqlDatabase::database("mainBase")));

    //для одежды используется другая модель, т.к. она комбинируется из роста и размера одежды
    clothesModel.reset(new QSqlQueryModel(this));
    clothesModel->setQuery(" SELECT Рост & '/' & РазмерОдежды AS Одежда "
                           " FROM _рост, _одежда "
                           " ORDER BY Рост & '/' & РазмерОдежды;", QSqlDatabase::database("mainBase"));


    //отображение содержимого склада
    stockModel.reset(new QSqlQueryModel(this));
    stockModel->setQuery(" SELECT наличие_на_складе.ИДНаимен as Код, наименования.Наимен as Наименование, наличие_на_складе.Размер, наличие_на_складе.Количество "
                         " FROM наименования INNER JOIN наличие_на_складе ON наименования.ИД = наличие_на_складе.ИДНаимен "
                         " ORDER BY наименования.Наимен, наличие_на_складе.Размер;", QSqlDatabase::database("mainBase"));
    stockModelFiltered.reset(new QSortFilterProxyModel(this));
    stockModelFiltered->setSourceModel(stockModel.data());
    stockModelFiltered->setFilterKeyColumn(1);
    stockModelFiltered->setFilterCaseSensitivity(Qt::CaseInsensitive);
    connect(ui->leFilterName, &QLineEdit::textChanged, stockModelFiltered.data(), &QSortFilterProxyModel::setFilterWildcard);

    ui->tvTable->setModel(stockModelFiltered.data());
    ui->tvTable->resizeColumnsToContents();
    ui->tvTable->setColumnWidth(1,530); //установить размеры столбцов, для корректного отображения
    ui->tvTable->setColumnWidth(3,30);
    ui->tvTable->resizeRowsToContents();





    connect(ui->cbID, &QComboBox::currentTextChanged, this, &winStock::refreshNamesAndSizes);
    connect(ui->cbName, &QComboBox::currentTextChanged, this, &winStock::refreshNamesAndSizes);
    connect(ui->cbSize, &QComboBox::currentTextChanged, this, &winStock::loadCount);

    connect(ui->tvTable, &QTableView::clicked, [&](QModelIndex x){
        ui->cbID->setCurrentText(x.siblingAtColumn(0).data().toString());
        ui->cbSize->setCurrentText(x.siblingAtColumn(2).data().toString());});






    connect(ui->pbSave, &QPushButton::clicked, this, &winStock::setNewVal);
    connect(ui->pbExport, &QPushButton::clicked, this, &winStock::exportTable);

    connect(ui->pbNames, &QPushButton::clicked, [&]{
        QDialog::accept();
        QScopedPointer<winNames> win(new winNames(nullptr));
        win->exec();});



    ui->cbID->currentTextChanged(ui->cbID->currentText()); //прогрузить первую запись
}

winStock::~winStock()
{
    delete ui;
}

void winStock::refreshNamesAndSizes(QString newVal)
{
    QComboBox* cb = qobject_cast<QComboBox*>(QObject::sender());//получить источник сигнала

    // в зависимости от источника установить столбец фильтрации
    if (cb->objectName() == "cbID")
        selectNameSortedModel->setFilterKeyColumn(0);
    if (cb->objectName() == "cbName")
        selectNameSortedModel->setFilterKeyColumn(1);

    //применить фильтр
    //для корректной обработки необходимо экранировать все служебные символы  . \ + * ? [ ^ ] $ ( ) { } = ! < > | : - #
    const static QStringList chars{"\\", ".",  "+", "*", "?", "[", "^", "]", "$", "(", ")", "{", "}", "=", "!", "<", ">", "|", ":", "-", "#"};
    for (QString c : chars)
        newVal.replace(c, "\\" + c);

    selectNameSortedModel->setFilterRegExp("^(" + newVal + ")$");

    if (selectNameSortedModel->rowCount() != 1)  return; //фильтровать до получения уникальной записи

    //применить изменения к другому боксу
    if (cb->objectName() == "cbID")
        ui->cbName->setCurrentText(selectNameSortedModel->index(0,1).data().toString());
    if (cb->objectName() == "cbName")
        ui->cbID->setCurrentText(selectNameSortedModel->index(0,0).data().toString());

    //заполнить поле размер для выбора
    if (selectNameSortedModel->index(0,4).data().toString().isEmpty()) return;

    QString type = selectNameSortedModel->index(0,4).data().toString().replace(' ','_').prepend("_").toLower(); //сформировать имя таблицы для извлечния из базы

    if(type == "_одежда")
        ui->cbSize->setModel(clothesModel.data());//для одежды другая модель
    else{
        ui->cbSize->setModel(sizeModel.data());
        sizeModel->setTable(type); //выбрать таблицу содерж размеры по типу
        sizeModel->select();
    }
    ui->cbSize->setCurrentIndex(0);
}

void winStock::loadCount()
{
    QSqlQuery q(QSqlDatabase::database("mainBase"));
    q.exec(" SELECT Количество "
           " FROM наличие_на_складе "
           " WHERE (ИДНаимен = '" + ui->cbID->currentText() + "' AND Размер = '" + ui->cbSize->currentText() +"');");

    if(q.first())
        ui->sbCount->setValue(q.value(0).toInt());
    else
        ui->sbCount->setValue(0);
}

void winStock::setNewVal()
{
    QSqlQuery q(QSqlDatabase::database("mainBase"));

    q.exec(" UPDATE наличие_на_складе SET Количество = " + QString::number(ui->sbCount->value()) + " "
           " WHERE (ИДНаимен='" + ui->cbID->currentText() + "' AND Размер = '" + ui->cbSize->currentText() +"');");

    if(!q.numRowsAffected()){ //если запись не была обновлена, значит ее нет и необходимо добавить в базу
        q.exec(" INSERT INTO наличие_на_складе(ИДНаимен, Размер, Количество) "
               " VALUES('" + ui->cbID->currentText() + "', '" + ui->cbSize->currentText() +"', " + QString::number(ui->sbCount->value()) + ");");
    }

    if(q.lastError().type())
        QMessageBox::critical(nullptr, "Ошбика", q.lastError().text());


    stockModel->setQuery(stockModel->query().lastQuery(), QSqlDatabase::database("mainBase")); //обновить таблицу сналичием на складе
}

void winStock::exportTable()
{
    //выбрать куда сохранять
    QString sExcelFilePath = QFileDialog::getSaveFileName(this, "Экспорт таблицы...", "склад", "*.xlsx");
    if (sExcelFilePath.isEmpty()) return;

    //удалить, если уже имеется
    QFile::remove(sExcelFilePath);
    QFile::copy(QCoreApplication::applicationDirPath() + "/tmpl/stock.xlsx", sExcelFilePath);


    ui->tvTable->selectAll();
    QModelIndexList indexes = ui->tvTable->selectionModel()->selectedRows();

    //создание екселя
    QScopedPointer<QAxObject> excel(new QAxObject("Excel.Application", this));
    QScopedPointer<QAxObject> app(excel->querySubObject("Application()"));
    QScopedPointer<QAxObject> wbks(excel->querySubObject("Workbooks()"));
    QScopedPointer<QAxObject> wb(wbks->querySubObject("Open(const QString&)", sExcelFilePath));
    QScopedPointer<QAxObject> ws(wb->querySubObject("Worksheets(int)", 1));



    //построить массив для выгрузки
    QList<QVariant> tableResult, columnResult;

    for (int r = 0 ; r < stockModel->rowCount(); r++){
        columnResult.clear();
        for (int c = 0; c < stockModel->columnCount(); c++) {
            columnResult << stockModel->index(r,c).data();
        }
        tableResult << QVariant(columnResult);
    }

    //показать окно
    app->setProperty("Visible", true);

    // получение диапазона ячеек
    QScopedPointer<QAxObject> Cell1(ws->querySubObject("Cells(QVariant&,QVariant&)", 2, 1));
    QScopedPointer<QAxObject> Cell2(ws->querySubObject("Cells(QVariant&,QVariant&)", tableResult.count() + 1, stockModel->columnCount()));
    QScopedPointer<QAxObject> range(ws->querySubObject("Range(const QVariant&,const QVariant&)", Cell1->asVariant(), Cell2->asVariant()));

    //заполнить ячейки
    range->setProperty("Value", tableResult);

    //перенос и авторазмер строк
    QScopedPointer<QAxObject> rows(ws->querySubObject("Rows()"));
    rows->dynamicCall("SetAutoFit", true);
    rows->dynamicCall("WrapText", true);

    //добавить строку итогов
    QScopedPointer<QAxObject> tbl(ws->querySubObject("ListObjects(\"tbl\")"));
    tbl->dynamicCall("ShowTotals", true);

    //сохранить
    wb->dynamicCall("Save");
}

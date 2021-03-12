#include "winnorma.h"
#include "ui_winnorma.h"

winNorma::winNorma(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::winNorma)
{
    ui->setupUi(this);

    //**********************************************первая вкладка
    //все пункты норм
    pointsModel.reset(new QSqlTableModel(this, QSqlDatabase::database("mainBase")));
    pointsModel->setTable("пункты_типовых_норм");
    pointsModel->select();
    ui->cbPoints->setModel(pointsModel.data());

    //все наименования для выбора
    allModel.reset(new QSqlQueryModel(this)); //ИД нам понадобится для подстановки при добавлении новых записей
    allModel->setQuery(" SELECT Наимен, ИД "
                       " FROM наименования "
                       " ORDER BY Наимен; ", QSqlDatabase::database("mainBase"));

    ui->tvAll->setModel(allModel.data());
    ui->tvAll->hideColumn(1); //спрятать столбец с ИД наименований
    ui->tvAll->horizontalHeader()->resizeSection(0,280);
    ui->tvAll->resizeRowsToContents();


    //наименования по выбранному пункту норм
    selectedModel.reset(new QSqlRelationalTableModel(this, QSqlDatabase::database("mainBase")));
    selectedModel->setTable("наименование_по_нормам");
    selectedModel->setRelation(2, QSqlRelation("наименования", "ИД", "Наимен")); //подгрузить данные из связной таблицы

    ui->tvSelected->setModel(selectedModel.data());
    ui->tvSelected->hideColumn(0); //спрятать столбец с кодом записи
    ui->tvSelected->hideColumn(1); //спрятать столбец с пунктом. он и так в выше виден
    ui->tvSelected->horizontalHeader()->resizeSection(2,220);
    ui->tvSelected->horizontalHeader()->resizeSection(3,60);
    ui->tvSelected->sortByColumn(2, Qt::AscendingOrder);


    //загрузка выбранных наименований по норме
    connect(ui->cbPoints, &QComboBox::currentTextChanged, [&](QString newVal){
        selectedModel->setFilter(" Пункт_норм = '" + newVal + "' ");
        selectedModel->select();
        ui->tvSelected->resizeRowsToContents();});

    connect(ui->pbAdd, &QPushButton::clicked, this, &winNorma::add);
    connect(ui->tvAll, &QTableView::doubleClicked, this, &winNorma::add);

    connect(ui->pbRem, &QPushButton::clicked, this, &winNorma::rem);
    connect(ui->tvSelected, &QTableView::doubleClicked, this, &winNorma::rem);

    //загрузка данных по периодичности выдачи
    connect(ui->tvSelected, &QTableView::clicked, this, &winNorma::loadPeriod);
    connect(ui->pbSavePeriod, &QPushButton::clicked, this, &winNorma::savePeriod);

    //окно с доп таблицами
    connect(ui->pbEditPoints,   &QPushButton::clicked, [&](bool /*b*/){
        QDialog::accept(); //закрыть текущее окно
        QStringList tables {"пункты_типовых_норм"};
        QScopedPointer<winTables> win(new winTables(tables, nullptr));
        win->exec();});

    emit ui->cbPoints->currentTextChanged(ui->cbPoints->currentText()); //загрузить первую запись




    //**********************************************вторая вкладка
    //модель с цехами
    allUnits.reset(new QSqlQueryModel(this));
    allUnits->setQuery("SELECT Сокр FROM цех ORDER BY Сокр;", QSqlDatabase::database("mainBase"));
    ui->cbUnit->setModel(allUnits.data());

    //модель с должностями
    allSeats.reset(new QSqlQueryModel(this));
    allSeats->setQuery("SELECT Должность FROM должности ORDER BY Должность;", QSqlDatabase::database("mainBase"));
    ui->cbSeat->setModel(allSeats.data());
    
    //модель с пе
    pe.reset(new QSqlQueryModel(this));
    pe->setQuery("SELECT ПЕ FROM _ПЕ ORDER BY ПЕ;", QSqlDatabase::database("mainBase"));
    ui->cbPE->setModel(pe.data());
    
    
    //модель с нг
    ng.reset(new QSqlQueryModel(this));
    ng->setQuery("SELECT НГ FROM _НГ ORDER BY НГ;", QSqlDatabase::database("mainBase"));
    ui->cbNG->setModel(ng.data());
    

    //используется табличный вид для того, чтобы отобразить всю должносьт, иначе она будет урезана
    QTableView* allSeatsView = new QTableView(this);
    allSeatsView->setModel(allSeats.data());
    allSeatsView->horizontalHeader()->setVisible(false);
    allSeatsView->verticalHeader()->setVisible(false);
    allSeatsView->resizeColumnsToContents();
    allSeatsView->resizeRowsToContents();
    allSeatsView->setSelectionMode(QAbstractItemView::SingleSelection);
    allSeatsView->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->cbSeat->setView(allSeatsView);
    //след строка увеличивает размер выпадающего меню до самой длинной строки с должностью
    ui->cbSeat->view()->setMinimumWidth(allSeatsView->columnWidth(0) + (allSeats->rowCount() > ui->cbSeat->maxVisibleItems() ? qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent) : 0));

    //модель пунктов
    ui->tvAllPoints->setModel(pointsModel.data());
    ui->tvAllPoints->horizontalHeader()->resizeSection(0,280);
    ui->tvAllPoints->resizeRowsToContents();

    selectedPoints.reset(new QSqlTableModel(this, QSqlDatabase::database("mainBase")));
    selectedPoints->setTable("пункты_по_должностям");

    ui->tvSelectedPoints->setModel(selectedPoints.data());
    ui->tvSelectedPoints->hideColumn(0);
    ui->tvSelectedPoints->hideColumn(1);
    ui->tvSelectedPoints->hideColumn(2);
    ui->tvSelectedPoints->horizontalHeader()->resizeSection(2,220);
    ui->tvSelectedPoints->horizontalHeader()->resizeSection(3,60);
    ui->tvSelectedPoints->sortByColumn(3, Qt::AscendingOrder);

    //модель для отображения всех наименований по выбранным пунктам
    selectedPointsShowNames.reset(new QSqlRelationalTableModel(this, QSqlDatabase::database("mainBase")));
    selectedPointsShowNames->setTable("наименование_по_нормам");
    selectedPointsShowNames->setRelation(2, QSqlRelation("наименования", "ИД", "Наимен")); //подгрузить данные из связной таблицы

    ui->tvResult->setModel(selectedPointsShowNames.data());
    ui->tvResult->hideColumn(0);
    ui->tvResult->hideColumn(1);

    //загрузка выбранных пунктов по цеху и должности
    auto loadPoints = [&](QString /*newValue*/){
        selectedPoints->setFilter("Цех = '" + ui->cbUnit->currentText() + "' AND Должность = '" + ui->cbSeat->currentText() + "'");
        selectedPoints->select();
        
        QSqlQuery q("SELECT ПЕ, НГ FROM категории WHERE Цех='" + ui->cbUnit->currentText() + "' AND Должность='" + ui->cbSeat->currentText() + "';", QSqlDatabase::database("mainBase"));
        if(q.first()){
            ui->cbPE->setCurrentText(q.value(0).toString());
            ui->cbNG->setCurrentText(q.value(1).toString());
        }else{
            ui->cbPE->setCurrentText("-");
            ui->cbNG->setCurrentText("0");
        }
        
        this->loadAllPerSelectedPoints();};//обновить список наименований по выбранным пунктам

    connect(ui->cbUnit, &QComboBox::currentTextChanged, loadPoints);
    connect(ui->cbSeat, &QComboBox::currentTextChanged, loadPoints);

    connect(ui->pbAddPoint, &QPushButton::clicked, this, &winNorma::addPoint);
    connect(ui->tvAllPoints, &QTableView::doubleClicked, this, &winNorma::addPoint);

    connect(ui->pbRemPoint, &QPushButton::clicked, this, &winNorma::remPoint);
    connect(ui->tvSelectedPoints, &QTableView::doubleClicked, this, &winNorma::remPoint);
    
    connect(ui->pbSavePE, &QPushButton::clicked, [&]{
        QSqlQuery q("", QSqlDatabase::database("mainBase"));
        q.exec("UPDATE категории SET ПЕ = '" + ui->cbPE->currentText() + "' WHERE Цех='" + ui->cbUnit->currentText() + "' AND Должность='" + ui->cbSeat->currentText() + "';");
        if(!q.numRowsAffected())
            q.exec("INSERT INTO категории (ПЕ, НГ, Цех, Должность) VALUES ('" + ui->cbPE->currentText() + "', '" + ui->cbNG->currentText() + "', '" + ui->cbUnit->currentText() + "', '" + ui->cbSeat->currentText() + "');");
    });
    
    connect(ui->pbSaveNG, &QPushButton::clicked, [&]{
        QSqlQuery q("", QSqlDatabase::database("mainBase"));
        q.exec("UPDATE категории SET  НГ = '" + ui->cbNG->currentText() + "' WHERE Цех='" + ui->cbUnit->currentText() + "' AND Должность='" + ui->cbSeat->currentText() + "';");
        if(!q.numRowsAffected())
            q.exec("INSERT INTO категории (ПЕ, НГ, Цех, Должность) VALUES ('" + ui->cbPE->currentText() + "', '" + ui->cbNG->currentText() + "', '" + ui->cbUnit->currentText() + "', '" + ui->cbSeat->currentText() + "');");
    });

    emit ui->cbUnit->currentTextChanged(ui->cbUnit->currentText()); //обновить содержимое выбранных пунктов
}

winNorma::~winNorma()
{
    delete ui;
}

void winNorma::add()
{
    if (!ui->tvAll->selectionModel()->hasSelection()) return;

    selectedModel->insertRow(0);
    selectedModel->setData(selectedModel->index(0,1), ui->cbPoints->currentText());
    selectedModel->setData(selectedModel->index(0,2), ui->tvAll->selectionModel()->selectedRows().at(0).siblingAtColumn(1).data().toString());
    selectedModel->submitAll();
    selectedModel->select();
    ui->tvSelected->resizeRowsToContents();
}

void winNorma::rem()
{
    if (!ui->tvSelected->selectionModel()->hasSelection()) return;

    selectedModel->removeRow(ui->tvSelected->selectionModel()->selectedRows().at(0).row());
    selectedModel->submitAll();
    selectedModel->select();
    ui->tvSelected->resizeRowsToContents();
}

void winNorma::loadPeriod(QModelIndex x)
{
    QString period = x.siblingAtColumn(3).data().toString();
    if (period.isEmpty()) return;

    if (period == "Дежурный")
        ui->rbDuty->setChecked(true);
    else if (period == "До износа")
        ui->rbWear->setChecked(true);
    else {
        ui->rbPeriodic->setChecked(true);
        ui->sbCount->setValue(period.split(" ").at(0).toInt());
        ui->cbPeriod->setCurrentText(period.split(" ").at(1));
    }
}

void winNorma::savePeriod()
{
    if (!ui->tvSelected->selectionModel()->hasSelection()) return;

    int row = ui->tvSelected->selectionModel()->selectedRows().at(0).row();

    if (ui->rbDuty->isChecked())
        selectedModel->setData(selectedModel->index(row,3), "Дежурный");
    else if (ui->rbWear->isChecked())
        selectedModel->setData(selectedModel->index(row,3), "До износа");
    else
        selectedModel->setData(selectedModel->index(row,3), ui->sbCount->text() + " " + ui->cbPeriod->currentText());

    selectedModel->submitAll();
    selectedModel->select();
    ui->tvSelected->selectRow(row);
}

void winNorma::addPoint()
{
    if (!ui->tvAllPoints->selectionModel()->hasSelection()) return;

    selectedPoints->insertRow(0);
    selectedPoints->setData(selectedPoints->index(0,1), ui->cbUnit->currentText());
    selectedPoints->setData(selectedPoints->index(0,2), ui->cbSeat->currentText());
    selectedPoints->setData(selectedPoints->index(0,3), ui->tvAllPoints->selectionModel()->selectedRows().at(0).data().toString());
    selectedPoints->submitAll();
    selectedPoints->select();
    ui->tvSelectedPoints->resizeRowsToContents();
    loadAllPerSelectedPoints();
}

void winNorma::remPoint()
{
    if (!ui->tvSelectedPoints->selectionModel()->hasSelection()) return;

    selectedPoints->removeRow(ui->tvSelectedPoints->selectionModel()->selectedRows().at(0).row());
    selectedPoints->submitAll();
    selectedPoints->select();
    ui->tvSelectedPoints->resizeRowsToContents();
    loadAllPerSelectedPoints();
}

void winNorma::loadAllPerSelectedPoints()
{
    QStringList filter;
    QAbstractItemModel* model = ui->tvSelectedPoints->model();
    for (int i = 0; i < model->rowCount(); ++i) {
        filter << " Пункт_норм = '" + model->index(i,3).data().toString() + "' ";
    }

    if (filter.isEmpty()) filter << "empty";

    selectedPointsShowNames->setFilter(filter.join(" OR "));
    selectedPointsShowNames->select();

    ui->tvResult->resizeColumnsToContents();
    ui->tvResult->resizeRowsToContents();
}

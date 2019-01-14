#include "winreport.h"
#include "ui_winreport.h"

winReport::winReport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::winReport)
{
    ui->setupUi(this);

    ui->pbrStatus->hide();

    ui->sbFrom->setValue(QDate::currentDate().year() - 1);
    ui->sbTo->setValue(QDate::currentDate().year() + 5);



    connect(ui->pbMake, &QPushButton::clicked, this, &winReport::makeReport);
}

winReport::~winReport()
{
    delete ui;
}

void winReport::makeReport()
{
    ui->pbrStatus->show();
    ui->pbMake->setEnabled(false);

    //выставить диапазон для расчета
    myRow::setYearStart(ui->sbFrom->value());
    myRow::setYearStop(ui->sbTo->value());

    BUF->updateTables();

    //первая половина отчета с прогнозируемыми данными
    QSqlQuery periodicTableQuery(" SELECT наименования.Наимен, наименование_по_нормам.Норма, наименования.Ед, наименования.Признак, наименования.Тип, работники.ТабНомер, работники.Цех, пункты_по_должностям.Должность, работники.Фамилия & ' ' & работники.Имя & ' ' & работники.Отчество AS ФИО, '0' AS Размер, выдано.ДатаВыдачи "
                                " FROM цех  "
                                " INNER JOIN ((пункты_типовых_норм  "
                                " INNER JOIN ((наименования  "
                                " INNER JOIN выдано ON наименования.ИД = выдано.ИДНаимен)  "
                                " INNER JOIN наименование_по_нормам ON наименования.ИД = наименование_по_нормам.ИДНаимен) ON пункты_типовых_норм.Пункт = наименование_по_нормам.Пункт_норм)  "
                                " INNER JOIN ((должности  "
                                " INNER JOIN пункты_по_должностям ON должности.Должность = пункты_по_должностям.Должность)  "
                                " INNER JOIN работники ON должности.Должность = работники.Должность) ON (работники.ТабНомер = выдано.ТабНомер) AND (пункты_типовых_норм.Пункт = пункты_по_должностям.Пункт_норм)) ON (цех.Сокр = работники.Цех) AND (цех.Сокр = пункты_по_должностям.Цех) "
                                " ORDER BY наименования.Наимен, работники.Цех, пункты_по_должностям.Должность; ", QSqlDatabase::database("mainBase"));


    qDebug() << "Формирование таблицы";
    QScopedPointer<myTable> periodicTable(new myTable(&periodicTableQuery, true));
    if (periodicTable->rowCount() == 0){
        QMessageBox::critical(nullptr, "Ошибка расчетной таблицы", "Ошибка формирования отчета. Проверьте правильность данных в БД");
        return;
    }



    qDebug() << "Заполнение таблицы Excel";
    QAxObject* excel = new QAxObject("Excel.Application");
    QAxObject* app = excel->querySubObject("Application()");
    QAxObject* wbks = excel->querySubObject("Workbooks()");
    QAxObject* wb = wbks->querySubObject("Open(const QString&)", QCoreApplication::applicationDirPath() + "/tmpl/result.xltm" );

    QAxObject* ws1 = wb->querySubObject("Worksheets(int)", 1);
    makeTableHead(ws1); //достроить шапку
    excel->dynamicCall("Run(QVariant)", "resTbl"); //расширить таблицу по всей ширине записей

    // получение диапазона ячеек
    QAxObject* Cell1_1 = ws1->querySubObject("Cells(QVariant&,QVariant&)", 4, 1);
    QAxObject* Cell2_1 = ws1->querySubObject("Cells(QVariant&,QVariant&)", periodicTable->rowCount() + 3, periodicTable->columnCount());
    QAxObject* range_1 = ws1->querySubObject("Range(const QVariant&,const QVariant&)", Cell1_1->asVariant(), Cell2_1->asVariant());

    //заполнить ячейки
    range_1->setProperty("Value", periodicTable->getTable<QVariant>());

    // получение диапазона ячеек
    QAxObject* ws2 = wb->querySubObject("Worksheets(int)", 2);
    QAxObject* Cell1_2 = ws2->querySubObject("Cells(QVariant&,QVariant&)", 3, 1);
    QAxObject* Cell2_2 = ws2->querySubObject("Cells(QVariant&,QVariant&)", RESULT->rCount() + 2, RESULT->cCount() - 1);
    QAxObject* range_2 = ws2->querySubObject("Range(const QVariant&,const QVariant&)", Cell1_2->asVariant(), Cell2_2->asVariant());

    //заполнить ячейки
    range_2->setProperty("Value", RESULT->getResult());


    //вторая часть отчета
    RESULT->resetResult();

    //данный запрос выгружает всех, кому необходимо выдать с датами выдачи (если были выданы)
    QSqlQuery onceGivenQuery(" SELECT TABLE1.Наимен, TABLE1.Норма, TABLE1.Ед, TABLE1.Признак, TABLE1.Тип, TABLE1.ТабНомер, TABLE1.Цех, TABLE1.Должность, TABLE1.ФИО, TABLE1.Размер,  IIf(IsNull(TABLE2.ДатаВыдачи), 'Не выдано', TABLE2.ДатаВыдачи) AS дата_выдачи "
                 " FROM (SELECT наименования.Наимен, наименование_по_нормам.Норма, наименования.Ед, наименования.Признак, наименования.Тип, работники.ТабНомер, работники.Цех, работники.Должность, [работники].[Фамилия] & ' ' & [работники].[Имя] & ' ' & [работники].[Отчество] AS ФИО, '0' AS Размер "
                 " FROM цех  "
                 " INNER JOIN ((пункты_типовых_норм  "
                 " INNER JOIN (наименования  "
                 " INNER JOIN наименование_по_нормам ON наименования.ИД = наименование_по_нормам.ИДНаимен) ON пункты_типовых_норм.Пункт = наименование_по_нормам.Пункт_норм)  "
                 " INNER JOIN ((должности  "
                 " INNER JOIN пункты_по_должностям ON должности.Должность = пункты_по_должностям.Должность)  "
                 " INNER JOIN работники ON должности.Должность = работники.Должность) ON пункты_типовых_норм.Пункт = пункты_по_должностям.Пункт_норм) ON (цех.Сокр = работники.Цех) AND (цех.Сокр = пункты_по_должностям.Цех) "
                 " WHERE (((наименование_по_нормам.Норма)='до износа' Or (наименование_по_нормам.Норма)='дежурный')) "
                 " ORDER BY наименование_по_нормам.Норма, работники.Цех, работники.Должность)  AS TABLE1 LEFT JOIN (SELECT наименования.Наимен, наименование_по_нормам.Норма, работники.ТабНомер, выдано.ДатаВыдачи "
                 " FROM (цех  "
                 " INNER JOIN ((пункты_типовых_норм  "
                 " INNER JOIN ((наименования  "
                 " INNER JOIN выдано ON наименования.ИД = выдано.ИДНаимен) "
                 " INNER JOIN наименование_по_нормам ON наименования.ИД = наименование_по_нормам.ИДНаимен) ON пункты_типовых_норм.Пункт = наименование_по_нормам.Пункт_норм)  "
                 " INNER JOIN (должности  "
                 " INNER JOIN пункты_по_должностям ON должности.Должность = пункты_по_должностям.Должность) ON пункты_типовых_норм.Пункт = пункты_по_должностям.Пункт_норм) ON цех.Сокр = пункты_по_должностям.Цех)  "
                 " INNER JOIN работники ON (цех.Сокр = работники.Цех) AND (работники.ТабНомер = выдано.ТабНомер) AND (должности.Должность = работники.Должность) "
                 " WHERE (((наименование_по_нормам.Норма)='до износа' Or (наименование_по_нормам.Норма)='дежурный')))  AS TABLE2 ON (TABLE1.Наимен = TABLE2.Наимен) AND (TABLE1.Норма = TABLE2.Норма) AND (TABLE1.ТабНомер = TABLE2.ТабНомер) "
                 " ORDER BY TABLE1.Наимен, TABLE1.Цех, TABLE1.Должность;", QSqlDatabase::database("mainBase"));


    QScopedPointer<myTable> onceGiven(new myTable(&onceGivenQuery, false));

    if (onceGiven->rowCount() == 0){
        QMessageBox::critical(nullptr, "Ошибка таблицы одиночной выдачи", "Ошибка формирования отчета. Проверьте правильность данных в БД");
        return;
    }


    // получение диапазона ячеек
    QAxObject* ws5 = wb->querySubObject("Worksheets(int)", 5);
    QAxObject* Cell1_5 = ws5->querySubObject("Cells(QVariant&,QVariant&)", 4, 1);
    QAxObject* Cell2_5 = ws5->querySubObject("Cells(QVariant&,QVariant&)", onceGiven->rowCount() + 3, onceGiven->columnCount());
    QAxObject* range_5 = ws5->querySubObject("Range(const QVariant&,const QVariant&)", Cell1_5->asVariant(), Cell2_5->asVariant());

    //заполнить ячейки
    range_5->setProperty("Value", onceGiven->getTable<QVariant>());


    // получение диапазона ячеек
    QAxObject* ws6 = wb->querySubObject("Worksheets(int)", 6 );
    QAxObject* Cell1_6 = ws6->querySubObject("Cells(QVariant&,QVariant&)", 3, 1);
    QAxObject* Cell2_6 = ws6->querySubObject("Cells(QVariant&,QVariant&)", RESULT->rCount() + 2, RESULT->cCount() - 1);
    QAxObject* range_6 = ws6->querySubObject("Range(const QVariant&,const QVariant&)", Cell1_6->asVariant(), Cell2_6->asVariant());

    //заполнить ячейки
    range_6->setProperty("Value", RESULT->getResult());

    //добавить итоговые строки в таблицы
    excel->dynamicCall("Run(QVariant)", "totals");

    //показать excel
    app->setProperty("Visible", true );

    excel->deleteLater();

    qDebug() << "Завершено";
    ui->pbrStatus->hide();
    ui->pbMake->setEnabled(true);
}

void winReport::makeTableHead(QAxObject* ws)
{
    int startCol = 12; //из файла excel
    int yStart = myRow::getYearStart();
    int yStop = myRow::getYearStop();
    QAxObject* Cell1,* Cell2,* range;
    QList<QVariant> row;

    //первая строка
    row.clear();

    int kolvo = yStop - yStart;
    int left = startCol;
    int right = left + kolvo;

    //по годам
    for (int n = 0 ; n < 3 ; n++){
        Cell1 = ws->querySubObject("Cells(QVariant&,QVariant&)", 1, left);
        Cell2 = ws->querySubObject("Cells(QVariant&,QVariant&)", 1, right);
        range = ws->querySubObject("Range(const QVariant&,const QVariant&)", Cell1->asVariant(), Cell2->asVariant());

        //заполнить ячейки
        range->dynamicCall("Select()");
        range->dynamicCall("MergeCells", true);
        range->dynamicCall("HorizontalAlignment", -4108);
        range->dynamicCall("VerticalAlignment", -4108);

        //Обводка жирным ппрямоугольником
        range->querySubObject("Borders(int)",7)->setProperty("Weight", -4138);
        range->querySubObject("Borders(int)",8)->setProperty("Weight", -4138);
        range->querySubObject("Borders(int)",9)->setProperty("Weight", -4138);
        range->querySubObject("Borders(int)",10)->setProperty("Weight", -4138);

        switch (n) {
        case 0:
            range->setProperty("Value", "График выдачи");
            break;
        case 1:
            range->setProperty("Value", "Потребность по годам");
            break;
        case 2:
            range->setProperty("Value", "Цена");
            break;
        default:
            break;
        }

        left += kolvo + 1;
        right = left + kolvo;
    }


    //по меясяцам
    right = left + 12; //выровнять правую границу для отображения

    for (int n = 0 ; n < 2 ; n++)
        for (int y = yStart; y <= yStop; y++){
            Cell1 = ws->querySubObject("Cells(QVariant&,QVariant&)", 1, left);
            Cell2 = ws->querySubObject("Cells(QVariant&,QVariant&)", 1, right);
            range = ws->querySubObject("Range(const QVariant&,const QVariant&)", Cell1->asVariant(), Cell2->asVariant());

            //соеденить ячейки
            range->dynamicCall("Select()");
            range->dynamicCall("MergeCells", true);
            range->dynamicCall("HorizontalAlignment", -4108);
            range->dynamicCall("VerticalAlignment", -4108);

            //Обводка жирным ппрямоугольником
            range->querySubObject("Borders(int)",7)->setProperty("Weight", -4138);
            range->querySubObject("Borders(int)",8)->setProperty("Weight", -4138);
            range->querySubObject("Borders(int)",9)->setProperty("Weight", -4138);
            range->querySubObject("Borders(int)",10)->setProperty("Weight", -4138);

            switch (n) {
            case 0:
                range->setProperty("Value", "Потребность по месяцам за " + QString::number(y));
                break;
            case 1:
                range->setProperty("Value", "Затраты по месяцам за " + QString::number(y));
                break;
            default:
                break;
            }

            left += 13;
            right = left + 12;
        }


    //вторая строка
    row.clear();
    for (int n = 0 ; n < 3 ; n++)
        for (int i = yStart; i <= yStop; i++)
            row << i;

    //по месяцам
    for (int y = yStart; y <= yStop; y++)
        for (int i = 1; i <= 13; i++)
            i == 13 ? row << "Итого" : row << i;

    for (int y = yStart; y <= yStop; y++)
        for (int i = 1; i <= 13; i++)
            i == 13 ? row << "Итого" : row << i;

    // получение диапазона ячеек
    Cell1 = ws->querySubObject("Cells(QVariant&,QVariant&)", 2, startCol);
    Cell2 = ws->querySubObject("Cells(QVariant&,QVariant&)", 2, startCol + row.count() - 1);
    range = ws->querySubObject("Range(const QVariant&,const QVariant&)", Cell1->asVariant(), Cell2->asVariant());


    //заполнить ячейки
    range->setProperty("Value", row);
}



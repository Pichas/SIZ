#include "mybuffer.h"


myBuffer::myBuffer(QObject *parent) : inMemDB(parent)
{
    clearBuffer();
}

void myBuffer::clearBuffer()
{
    QSqlQuery q("", memdb);
    q.exec("DROP TABLE workers");
    q.exec("DROP TABLE prices");
    q.exec("DROP TABLE stock");


    //создаем базы для хранения нужных таблиц
    q.exec("CREATE TABLE workers ( " //данные по работникам
           "tNum INTEGER PRIMARY KEY, "
           "dekret INTEGER, "
           "dataPost TEXT, "
           "dataPerev TEXT, "
           "pol TEXT, "
           "odejda TEXT, "
           "obuv TEXT, "
           "golova TEXT, "
           "protivogaz TEXT, "
           "respir TEXT, "
           "rukavica TEXT, "
           "perch TEXT, "
           "cvet TEXT, "
           "bezRazmera TEXT)");

    q.exec("CREATE TABLE prices ( " //данные по ценам
           "id INTEGER PRIMARY KEY, "
           "naimen TEXT, "
           "god INTEGER, "
           "cena INTEGER)");

    q.exec("CREATE TABLE stock ( " //данные по складу
           "id TEXT PRIMARY KEY, "
           "naimen TEXT, "
           "razmer TEXT, "
           "kolvo INTEGER)");
    
    q.exec("CREATE TABLE kats ( " //данные по типам
           "id TEXT PRIMARY KEY, "
           "cex TEXT, "
           "dolj TEXT, "
           "PE TEXT, "
           "NG INTEGER)");
    
}

void myBuffer::insertInMemDB(QString tblName, QSqlQuery* queryExt, QSqlQuery* queryMem)
{
    beginTransaction();
    while (queryExt->next()){
        QStringList values;
        for (int i = 0; i < queryExt->record().count(); ++i) {
            values << queryExt->value(i).toString();
        }
        queryMem->exec("INSERT INTO " + tblName + " VALUES ('" + values.join("', '") + "')");
    }
    endTransaction();
}


void myBuffer::updateTables()
{
    clearBuffer(); //очистить таблицы перед новым отчетом

    QSqlQuery qMem("", memdb);
    QSqlQuery qExt("", QSqlDatabase::database("mainBase"));

    //загрузка работников
    qExt.exec(" SELECT ТабНомер, "
              " Декрет, "
              " ДатаПоступления, "
              " ДатаПеревода, "
              " Пол, "
              " Рост & '/' & РазмерОдежды AS [Одежда], "
              " РазмерОбуви AS [Обувь], "
              " РазмерГоловногоУбора AS [Головной убор], "
              " РазмерПротивогаза AS [Противогаз], "
              " размерРеспиратора AS [Респиратор], "
              " РазмерРукавиц AS [Рукавицы], "
              " РазмерПерчаток AS [Перчатки], "
              " ЦветКаски AS [Цвет], "
              " '-' AS [Без размера] "
              " FROM работники "
              " ORDER BY ТабНомер; ");

    insertInMemDB("workers", &qExt, &qMem);

    //сопоставить индексы с текстовым именем
    indexWorker.clear();
    indexWorker["Одежда"] = qExt.record().indexOf("Одежда");
    indexWorker["Обувь"] = qExt.record().indexOf("Обувь");
    indexWorker["Головной убор"] = qExt.record().indexOf("Головной убор");
    indexWorker["Противогаз"] = qExt.record().indexOf("Противогаз");
    indexWorker["Респиратор"] = qExt.record().indexOf("Респиратор");
    indexWorker["Рукавицы"] = qExt.record().indexOf("Рукавицы");
    indexWorker["Перчатки"] = qExt.record().indexOf("Перчатки");
    indexWorker["Цвет"] = qExt.record().indexOf("Цвет");
    indexWorker["Без размера"] = qExt.record().indexOf("Без размера");
    indexWorker["Декрет"] = qExt.record().indexOf("Декрет");

    //загрузка цен
    qExt.exec(" SELECT цена.Код, наименования.Наимен, цена.Год, цена.Цена "
              " FROM наименования INNER JOIN цена ON наименования.ИД = цена.ИДНаим;");

    insertInMemDB("prices", &qExt, &qMem);

    //загрузка склада
    qExt.exec(" SELECT наличие_на_складе.Код, наименования.Наимен, наличие_на_складе.Размер, наличие_на_складе.Количество "
              " FROM наименования INNER JOIN наличие_на_складе ON наименования.ИД = наличие_на_складе.ИДНаимен;");

    insertInMemDB("stock", &qExt, &qMem);
    
    //загрузка категорий
    qExt.exec("SELECT * FROM категории;");

    insertInMemDB("kats", &qExt, &qMem);
}

QMap<int, int> myBuffer::getPrice(QString naim) const
{
    QSqlQuery q("SELECT god, cena FROM prices WHERE naimen = '" + naim + "'", memdb);
    //   год, цена
    QMap<int, int> result;
    while (q.next())
        result[q.value(0).toInt()] = q.value(1).toInt();
    return result;
}

QList<QVariant> myBuffer::getWorkerInfo(QString tNum) const
{
    QSqlQuery q("SELECT * FROM workers WHERE tNum = " + tNum, memdb);

    if (q.first()) {
        QList<QVariant> worker;
        for (int i = 0; i < q.record().count(); ++i)
            worker << q.value(i);
        return worker;
    }

    qDebug() << "Работник с номером " << tNum << " не найден";
    return QList<QVariant> ();
}

int myBuffer::getStockCount(QString naim, QString size) const
{
    QSqlQuery q("SELECT kolvo FROM stock WHERE naimen = '" + naim + "' AND razmer = '" + size + "'", memdb);
    return q.first() ? q.value(0).toInt() : 0;
}

QPair<QString, QString> myBuffer::getKat(QString cex, QString dolj)
{
        QSqlQuery q("SELECT PE, NG FROM kats WHERE cex = '" + cex + "' AND dolj = '"+ dolj + "';", memdb);
        
        if (q.first()) return QPair<QString, QString>(q.value(0).toString(),q.value(1).toString());
            
        return QPair<QString, QString>("-","0");
}

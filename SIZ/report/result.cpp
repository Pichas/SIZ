#include "result.h"


result::result(QObject *parent) : inMemDB(parent)
{
    resetResult();
}

void result::addEntry(QString cex, QString naim, QString priznak, QString razmer, int god, int kolvo,int cena, int sklad)
{
    QSqlQuery q("", memdb);

    q.exec("UPDATE result SET kolvo = kolvo + " + QString::number(kolvo) + " WHERE cex = '" + cex + "' AND naim = '" + naim + "' AND priznak = '" + priznak + "' AND razmer = '" + razmer + "' AND god = " + QString::number(god) + " ");
    if (q.numRowsAffected() == 0) //если не удалось обновить запись, значит ее нет и ее надо добавить
        q.exec("INSERT INTO result(cex, naim, priznak, razmer, god, kolvo, cena, sklad) "
               " VALUES('" + cex + "', '" + naim + "', '" + priznak + "', '" + razmer + "', " + QString::number(god) + ", " + QString::number(kolvo) + ", " + QString::number(cena) + ", " + QString::number(sklad) + ")");
}

QList<QVariant> result::getResult()
{
    QSqlQuery q("SELECT * FROM result ORDER BY god, naim, cex, razmer;", memdb);

    QList<QVariant> tableResult;
    while (q.next()){
        QList<QVariant> columnResult;
        for (int c = 1; c < q.record().count(); c++){
            columnResult << q.value(c);
        }
        tableResult << QVariant(columnResult);
    }

    return tableResult;
}

int result::rCount()
{
    QSqlQuery q("SELECT count(id) FROM result;", memdb);
    return q.first() ? q.value(0).toInt() : 0;
}

int result::cCount()
{
    return memdb.record("result").count();
}

void result::resetResult()
{
    QSqlQuery q("", memdb);

    q.exec("DROP TABLE result");

    q.exec("CREATE TABLE result ( "
           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
           "cex TEXT, "
           "naim TEXT, "
           "priznak TEXT, "
           "razmer TEXT, "
           "god INTEGER, "
           "kolvo INTEGER, "
           "cena INTEGER, "
           "sklad INTEGER )");
}


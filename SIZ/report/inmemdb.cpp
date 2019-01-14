#include "inmemdb.h"

QSqlDatabase inMemDB::memdb = QSqlDatabase();

inMemDB::inMemDB(QObject *parent) : QObject(parent)
{
    if(!memdb.isValid()) {
        memdb = QSqlDatabase::addDatabase("QSQLITE", "in_mem_db");
        memdb.setDatabaseName(":memory:");

        if (!memdb.open())
            qDebug() << tr("Unable to open database") << tr("An error occurred while opening the connection: ") << memdb.lastError().text();
        QSqlQuery q("PRAGMA synchronous = OFF", memdb); //выключить синхронизацию для увеличения скорости работы
    }
}

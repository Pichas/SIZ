#ifndef INMEMDB_H
#define INMEMDB_H

#include <QObject>
#include <QtSql>
#include <QDebug>

class inMemDB : public QObject
{
    Q_OBJECT
public:



    bool beginTransaction(){return memdb.transaction();}
    bool endTransaction(){return memdb.commit();}


protected:
    inMemDB(QObject *parent = nullptr);

    inMemDB(const inMemDB& Src);
    inMemDB(inMemDB&& Src);
    inMemDB& operator=(const inMemDB& Src);

    static QSqlDatabase memdb;

public slots:
};

#endif // INMEMDB_H

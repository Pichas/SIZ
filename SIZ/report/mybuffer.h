#ifndef MYBUFFER_H
#define MYBUFFER_H

#include <QObject>

#include "inmemdb.h"

class myBuffer;
#define BUF myBuffer::instance()

class myBuffer : public inMemDB
{
    Q_OBJECT
public:

    static myBuffer* instance(){
        static myBuffer* theSingleInstance = new myBuffer();
        return theSingleInstance;
    }

    void updateTables();

    QMap<int, int> getPrice(QString naim) const;
    QList<QVariant> getWorkerInfo(QString tNum) const;
    int getStockCount(QString naim, QString size) const;
    QMap<QString, int> getIndexWorker() const {return indexWorker; }

private:
    myBuffer(QObject *parent = nullptr);
    void clearBuffer();

    void insertInMemDB(QString tblName, QSqlQuery *queryExt, QSqlQuery *queryMem);



    //индексы полей для быстрого доступа
    QMap<QString, int> indexWorker;



signals:

public slots:
};

#endif // MYBUFFER_H

#ifndef MYTABLE_H
#define MYTABLE_H

#include <QObject>
#include <QtSql>
#include <QTimer>
#include <QSharedPointer>
#include <QScopedPointer>

#include "myrowsmall.h"
#include "myrowlarge.h"
#include "myrowonce.h"

class myTable : public QObject
{
    Q_OBJECT
public:
    explicit myTable(QSqlQuery *q, bool calc = true, QObject *parent = nullptr);

    template<typename T>
    QList<T> getTable() const{
        QList<T> result;
        for(myRow* cell : rows)
            result << T(cell->getRow<T>());
        return result;
    }

    QList<myRow*> getTable() const{
        return rows;
    }


    myRow* getRow(int i){
        return rows[i];
    }



    int rowCount() const {return rows.count();}
    int columnCount() const {return rows.at(0)->columnCount();}

private:
    QList<myRow*> rows;

signals:

public slots:
};

#endif // MYTABLE_H

#ifndef MYROW_H
#define MYROW_H

#include <QObject>
#include <QtSql>
#include <QDateTime>
#include <QScopedPointer>

#include "mycell.h"
#include "result.h"
#include "mybuffer.h"

class myRow : public QObject
{
    Q_OBJECT
public:
    explicit myRow(const QList<QVariant> rec, QObject *parent = nullptr);
    ~myRow();


    template<typename T>
    QList<T> getRow() const
    {
        QList<T> result;
        for(myCell* cell : cells)
            result << cell->getValue<T>();
        return result;
    }

    QList<myCell*> getRow() const{
        return cells;
    }

    myCell* getCell(int i){
        return cells[i];
    }

    int columnCount() const {return cells.count();}

    virtual void calculateRow();
    void onceGivenRow();

    static void setYearStart(int y){yearStart = y;}
    static void setYearStop(int y){yearStop = y;}
    static int getYearStart(){return yearStart;}
    static int getYearStop(){return yearStop;}

private:


protected:
    QList<myCell*> cells;

    QList<QVariant> worker;
    QMap<int, int> yearPrice;

    static int yearStart;
    static int yearStop;

    virtual void poGodam(int) = 0;
    virtual void poMesacam(int) = 0;

signals:

public slots:
};



#endif // MYROW_H

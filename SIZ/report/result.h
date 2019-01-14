#ifndef RESULT_H
#define RESULT_H

#include <QObject>

#include "inmemdb.h"

class result;
#define RESULT result::instance()

class result : public inMemDB
{
    Q_OBJECT
public:


    static result* instance(){
        static result* theSingleInstance = new result();
        return theSingleInstance;
    }

    void addEntry(QString cex, QString naim, QString tip, QString razmer, int god, int kolvo, int cena, int sklad);
    QList<QVariant> getResult();
    int rCount();
    int cCount();

    void resetResult();


private:
    result(QObject *parent = nullptr);
    ~result(){}



signals:

public slots:
};

#endif // RESULT_H

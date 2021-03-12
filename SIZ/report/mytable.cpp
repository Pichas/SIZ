#include "mytable.h"


myTable::myTable(QSqlQuery *q, bool calc, QObject *parent) : QObject(parent)
{
    QSharedPointer<QTimer> pTimer (new QTimer(), [](QTimer* obj){ //деструетор
        obj->stop();
        obj->deleteLater();});

    connect(pTimer.data(), &QTimer::timeout, [&](){
        qDebug() << "Обработка записи №" << rows.count();});

    pTimer->start(1000);
    
    QString errors;
    

    RESULT->beginTransaction();
    while (q->next()){
        QList<QVariant> record;
        for (int i = 0; i < q->record().count(); ++i) {
            record << q->value(i);
        }

        QString norma = record.at(1).toString();
        if (norma.isEmpty()) {
            qDebug() << "*********ОШИБКА В РАСЧЕТАХ*********";
            qDebug() << "Не указан период выдачи";
            qDebug() << record.at(0).toString();
            qDebug() << record.at(6).toString();
            qDebug() << record.at(7).toString();
            qDebug() << record.at(5).toString() << " " << record.at(8).toString();
            
            errors += "Ошибка: Не указан период выдачи " 
                                    + record.at(0).toString() + " " 
                                    + record.at(6).toString() + " " 
                                    + record.at(7).toString() + " " 
                                    + record.at(5).toString() + "\n\n" ;
            continue;
        }

        if(record.at(3).toString().isEmpty())
            errors += "ОШИБКА! НЕ УКАЗАНЫ ЕД. ИЗМЕРЕНИЯ " + record.at(0).toString() + "\r\n";
        if(record.at(4).toString().isEmpty())
            errors += "ОШИБКА! НЕ УКАЗАН ПРИЗНАК        " + record.at(0).toString() + "\r\n";
        if(record.at(5).toString().isEmpty())
            errors += "ОШИБКА! НЕ УКАЗАН ТИП            " + record.at(0).toString() + "\r\n";

        if (norma.contains(QRegExp("(Дежурный|До износа)")))
            rows.append(new myRowOnce(record, this)); //что считается один раз
        /*else if (record.at(12).toDate() < QDate(2021, 1, 1))
            rows.append(new myRowLarge(record, this)); //расчет большого периода*/
        else 
            rows.append(new myRowSmall(record, this)); //расчет маленького периода
        
        if(rows.last()->getCell(9)->getValue().toString().isEmpty())
            errors += "ОШИБКА! НЕ УКАЗАН РАЗМЕР            " + record.at(0).toString() + " " + record.at(8).toString() + "\r\n";
        

        if (calc)
            rows.last()->calculateRow();
        else {
            rows.last()->onceGivenRow();
        }
        //если требуется спрогнозировать, то выполнить расчеты.
        //иначе просто строит таблицу для подсчета факта выдачи
        //это используется для получения данных в категории "дежурный" и "до износа"

        qApp->processEvents(); //обрабтка всех событий
    }

    
    
    RESULT->endTransaction();
    
    if (!errors.isEmpty()) {
       QPlainTextEdit* pt = new QPlainTextEdit(nullptr);
       pt->setPlainText(errors);
       pt->show();
       pt->setWindowTitle("Ошибки");
       pt->setGeometry(100,100,500,300);
       
       qApp->processEvents(); //обрабтка всех событий
    }
}





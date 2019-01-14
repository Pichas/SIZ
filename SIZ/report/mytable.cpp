#include "mytable.h"

myTable::myTable(QSqlQuery *q, bool calc, QObject *parent) : QObject(parent)
{
    QSharedPointer<QTimer> pTimer (new QTimer(), [](QTimer* obj){ //деструетор
        obj->stop();
        obj->deleteLater();});

    connect(pTimer.data(), &QTimer::timeout, [&](){
        qDebug() << "Обработка записи №" << rows.count();});

    pTimer->start(1000);

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

            continue;
        }

        if(record.at(3).toString().isEmpty())
            qDebug() << "ОШИБКА! НЕ УКАЗАНЫ ЕД. ИЗМЕРЕНИЯ " << record.at(0).toString() << "\r\n";
        if(record.at(4).toString().isEmpty())
            qDebug() << "ОШИБКА! НЕ УКАЗАН ПРИЗНАК        " << record.at(0).toString() << "\r\n";
        if(record.at(5).toString().isEmpty())
            qDebug() << "ОШИБКА! НЕ УКАЗАН ТИП            " << record.at(0).toString() << "\r\n";

        if (norma.contains(QRegExp("(Дежурный|До износа)")))
            rows.append(new myRowOnce(record, this)); //что считается один раз
        else if (norma.split(" ").at(1).toInt() > 12)
            rows.append(new myRowLarge(record, this)); //расчет большого периода
        else if (norma.split(" ").at(1).toInt() <= 12)
            rows.append(new myRowSmall(record, this)); //расчет маленького периода

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
}





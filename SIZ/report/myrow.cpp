#include "myrow.h"

int myRow::yearStart = 0;
int myRow::yearStop = 0;

myRow::myRow(const QList<QVariant> rec, QObject *parent) : QObject(parent)
{
    worker =  BUF->getWorkerInfo(rec.at(5).toString()); //загрузить информацию по текущему работнику
    yearPrice = BUF->getPrice(rec.at(0).toString()); //загрузить цены к текущему наименованию
    QPair<QString, QString> kat = BUF->getKat(rec.at(6).toString(), rec.at(7).toString());

    for (int c = 0 ; c < rec.count() ; c++){
        if (c == 9){ //загрузить размер в колонку из таблицы работников
            cells.append(new myCell(worker.at(BUF->getIndexWorker().value(rec.at(4).toString(), 0)), this));
            continue;
        }
        
        if (c == 10){ //PE
            cells.append(new myCell(kat.first));
            continue;
        }
        
        if (c == 11){ //NG
            cells.append(new myCell(kat.second));
            continue;
        }
        
        
        if (c == 12){
            if (rec.at(12).toDate().day() > 29){ //если день выдачи больше 29, то ставим 28. все изза февраля
                cells.append(new myCell(QDate(rec.at(12).toDate().year(), rec.at(12).toDate().month(), 28), this));
                continue;
            }
            //если в декрете то дату выдачи заменить на надпись декрет
            if (worker.at(BUF->getIndexWorker().value("Декрет", 2)).toBool()){
                cells.append(new myCell("Декрет", this));
                continue;
            }
        }
        cells.append(new myCell(rec.at(c)));
    }
}

myRow::~myRow()
{
}


void myRow::calculateRow()
{
    if(cells.at(12)->getValue<QString>() == "Декрет") return;

    for (int i = 0; i <= 2; i++) poGodam(i); //годовые расчеты
    for (int i = 1; i <= 2; i++) poMesacam(i); //месячные расчеты
}

void myRow::onceGivenRow()
{
    RESULT->addEntry("Все",
                     cells.at(0)->getValue().toString(),//наименование
                     cells.at(3)->getValue().toString(), //признак
                     cells.at(9)->getValue().toString(), //размер
                     QDate::currentDate().year() + 1, //дата на след год
                     cells.at(12)->getValue().toString() == "Не выдано" ? 1 : 0, //количество
                     BUF->getPrice(cells.at(0)->getValue().toString()).value(QDate::currentDate().year() + 1, 0),
                     BUF->getStockCount(cells.at(0)->getValue().toString(), cells.at(9)->getValue().toString())
                );
}


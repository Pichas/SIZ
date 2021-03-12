#include "myrowperiodic.h"



myRowPeriodic::myRowPeriodic(const QList<QVariant> rec, QObject *parent) : myRow(rec, parent)
{
    dv = cells.at(12)->getValue<QDate>(); //дата выдачи
    dataYstr = worker.at(2).toDate(); //дата устройства на работу

    kolvo = cells.at(1)->getValue<QString>().split(" ").at(0).toInt(); //количество едениц
    period = cells.at(1)->getValue<QString>().split(" ").at(1).toInt(); //периодичность. тут она 12 месяцев
    skladKolvo = BUF->getStockCount(cells.at(0)->getValue<QString>(), cells.at(9)->getValue<QString>()); //получить наличие на складе
}

void myRowPeriodic::poGodam(int k)
{
    gotoYearStart();

    for (int year = yearStart; year <= yearStop; year++){
        cells.append(new myCell("", this)); //сразу добавляем ячейку для работы

        while (dv.year() == year){ //пока работаем в текущем году
            if(dv >= dataYstr){//работник уже устроен на расчетный период
                switch (k) { //выбор значения для записи в ячейку
                case 0: //план выдачи по годам
                    cells.last()->setValue(dv);
                    break;
                case 1: //потребность по годам
                    cells.last()->setValue(cells.last()->getValue<int>() + kolvo);

                    RESULT->addEntry("Все", //цех ----cells.at(6)->getValue<QString>()
                                     cells.at(0)->getValue<QString>(), //наименование
                                     cells.at(3)->getValue<QString>(), //признак
                                     cells.at(9)->getValue<QString>(), //размер
                                     year, //год
                                     kolvo, //количество потребности
                                     yearPrice.value(year), //цена в этом году
                                     skladKolvo); //склад
                    break;
                case 2: //цены по годам
                    cells.last()->setValue(yearPrice.value(dv.year(), 0));
                    break;
                default:
                    break;
                }
            } //если работник не устроен то просто переходим дальше
            dv = dv.addMonths(period); //переключиться на следующий период
        }
    }
}



#include "myrowsmall.h"

myRowSmall::myRowSmall(const QList<QVariant> rec, QObject *parent) : myRowPeriodic(rec, parent)
{
    //сокращаем период для выполнения более точного расчета
    switch (kolvo) {
    case 2:
    case 3:
    case 4:
    case 6:
    case 12:
        period /= kolvo;
        kolvo = 1;
        break;
    case 24:
        period = 1;
        kolvo = 2;
        break;
    default:
        break;
    }
}

void myRowSmall::poMesacam(int k)
{
    gotoYearStart();

    for (int year = yearStart; year <= yearStop; year++){ //обработать заданный диапазон лет
        int sum = 0;
        for (int month = 1; month <= 13; month++){ //каждый месяц + итог за год
            cells.append(new myCell("", this)); //сразу добавляем ячейку для работы

            if (month == 13){ //если итоговая строка, то вывести результат и перейти дальше
                cells.last()->setValue(sum);
                continue;
            }
            if (dv == QDate(year, month, dv.day())){ //если месяц обработки совпал с месяцем для выдачи
                if (dv >= dataYstr){  //если период больше даты устройства то посчитать
                    switch (k) { //выбор значения для записи в ячейку
                    case 1: //потребность по месяцам
                        cells.last()->setValue(kolvo);
                        break;
                    case 2: //цены по месяцам
                        cells.last()->setValue(yearPrice.value(dv.year(), 0) * kolvo);
                        break;
                    default:
                        break;
                    }
                    sum += cells.last()->getValue<int>();  //считать сумму за год
                } // если нет то просто нарастить период
                dv = dv.addMonths(period);
            }
        }
    }
}


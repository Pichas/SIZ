#include "myrowlarge.h"

myRowLarge::myRowLarge(const QList<QVariant> rec, QObject *parent) : myRowPeriodic(rec, parent)
{

}

void myRowLarge::poMesacam(int k)
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

            switch (k) {
            case 1://если расчет потребности
                if (dv == QDate(year, month, dv.day())){
                    if (dv >= dataYstr){  //если период больше даты устройства то посчитать
                        cells.last()->setValue(kolvo);
                        sum += cells.last()->getValue<int>();  //считать сумму за год
                    } // если нет то просто нарастить период
                    dv = dv.addMonths(period);
                }
                break;
            case 2://списания считаются по другому
                if (dv.addMonths(period) == QDate(year, month, dv.day())) //если пошел расчет след периода
                    dv = dv.addMonths(period); //то добавить его в расчетные формулы

                    if (dv >= dataYstr){  //если период больше даты устройства то посчитать
                        cells.last()->setValue(yearPrice.value(dv.year(), 0) * kolvo / /*(float)*/period); //если необходимо повысить точность
                        sum += cells.last()->getValue<int>();  //считать сумму за год
                    }
                break;
            default:
                break;
            }
        }
    }
}


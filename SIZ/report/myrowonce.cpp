#include "myrowonce.h"

myRowOnce::myRowOnce(const QList<QVariant> rec, QObject *parent) : myRow(rec, parent)
{

}

void myRowOnce::poGodam(int k)
{
    QDate dv = cells.at(12)->getValue<QDate>(); //дата выдачи

    for (int year = yearStart; year <= yearStop; year++){
        cells.append(new myCell("", this)); //сразу добавляем ячейку для работы

        if(dv.year() == year){
            switch (k) { //выбор значения для записи в ячейку
            case 0: //план выдачи по годам
                cells.last()->setValue(dv);
                break;
            case 1: //потребность по годам
                cells.last()->setValue(1);
                break;
            case 2: //цены по годам
                cells.last()->setValue(yearPrice.value(dv.year(), 0));
                break;
            default:
                break;
            }
        }
    }
}

void myRowOnce::poMesacam(int k)
{
    QDate dv = cells.at(12)->getValue<QDate>(); //дата выдачи

    for (int year = yearStart; year <= yearStop; year++){ //обработать заданный диапазон лет
        int sum = 0;
        for (int month = 1; month <= 13; month++){ //каждый месяц + итог за год
            cells.append(new myCell("", this)); //сразу добавляем ячейку для работы

            if (month == 13){ //если итоговая строка, то вывести результат и перейти дальше
                cells.last()->setValue(sum);
                continue;
            }

            if (dv == QDate(year, month, dv.day())){ //если дата обработки совпал с месяцем для выдачи
                switch (k) { //выбор значения для записи в ячейку
                case 1: //потребность по месяцам
                    cells.last()->setValue(1);
                    break;
                case 2: //цены по месяцам
                    cells.last()->setValue(yearPrice.value(dv.year(), 0));
                    break;
                default:
                    break;
                }
                sum += cells.last()->getValue<int>();  //считать сумму за год
            }
        }
    }
}



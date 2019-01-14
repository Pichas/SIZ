#ifndef MYROWPERIODIC_H
#define MYROWPERIODIC_H

#include "myrow.h"


class myRowPeriodic : public myRow
{
public:
    explicit myRowPeriodic(const QList<QVariant> rec, QObject *parent = nullptr);

protected:
    QDate dv; //дата выдачи
    QDate dataYstr; //дата устройства на работу

    int kolvo; //количество едениц
    int period; //периодичность. тут она 12 месяцев
    int skladKolvo; //количество на складе


    void gotoYearStart(){
        //выставляем на начало расчетного периода
        //сделано в две стороны и по периодам выдачи для того,
        //чтобы учесть то, что дано позже и намного раньше расчетного периода
        //ну и для учета, если период меньше года и выйти на начало расчетного года
        while (dv.year() >= yearStart)
            dv = dv.addMonths(-period);
        while (dv.year() < yearStart)
            dv = dv.addMonths(period);
    }

    virtual void poGodam(int k) override;
};

#endif // MYROWPERIODIC_H

#ifndef MYROWLARGE_H
#define MYROWLARGE_H

#include "myrowperiodic.h"

class myRowLarge : public myRowPeriodic
{
    Q_OBJECT
public:
    explicit myRowLarge(const QList<QVariant> rec, QObject *parent = nullptr);

private:
    virtual void poMesacam(int k) override;
};

#endif // MYROWLARGE_H

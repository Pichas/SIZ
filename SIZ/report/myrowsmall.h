#ifndef MYROWSMALL_H
#define MYROWSMALL_H

#include "myrowperiodic.h"

class myRowSmall : public myRowPeriodic
{
    Q_OBJECT
public:
    explicit myRowSmall(const QList<QVariant> rec, QObject *parent = nullptr);

private:
    virtual void poMesacam(int k) override;
};

#endif // MYROWSMALL_H

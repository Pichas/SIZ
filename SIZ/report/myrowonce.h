#ifndef MYROWONCE_H
#define MYROWONCE_H


#include "myrow.h"

class myRowOnce : public myRow
{
public:
    explicit myRowOnce(const QList<QVariant> rec, QObject *parent = nullptr);

private:
    virtual void poGodam(int k) override;
    virtual void poMesacam(int k) override;

};

#endif // MYROWONCE_H

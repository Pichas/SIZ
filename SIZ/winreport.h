#ifndef WINREPORT_H
#define WINREPORT_H

#include <QDialog>
#include <QAxObject>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QMessageBox>
#include <QLineEdit>

#include "report/result.h"
#include "report/mytable.h"

namespace Ui {
class winReport;
}

class winReport : public QDialog
{
    Q_OBJECT

public:
    explicit winReport(QWidget *parent = nullptr);
    ~winReport();

private:
    Ui::winReport *ui;

    void makeReport();
    void makeTableHead(QAxObject *ws);

};



#endif // WINREPORT_H

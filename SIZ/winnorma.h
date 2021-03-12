#ifndef WINNORMA_H
#define WINNORMA_H

#include <QDialog>
#include <QScopedPointer>
#include <QSqlRelationalTableModel>
#include <QSqlQuery>
#include <QSqlError>

#include "wintables.h"

namespace Ui {
class winNorma;
}

class winNorma : public QDialog
{
    Q_OBJECT

public:
    explicit winNorma(QWidget *parent = nullptr);
    ~winNorma();

private:
    Ui::winNorma *ui;

    QScopedPointer<QSqlTableModel> pointsModel;
    QScopedPointer<QSqlQueryModel> allModel;
    QScopedPointer<QSqlRelationalTableModel> selectedModel;

    QScopedPointer<QSqlQueryModel> allUnits;
    QScopedPointer<QSqlQueryModel> allSeats;
    QScopedPointer<QSqlTableModel> selectedPoints;
    QScopedPointer<QSqlRelationalTableModel> selectedPointsShowNames;

    
    QScopedPointer<QSqlQueryModel> pe;
    QScopedPointer<QSqlQueryModel> ng;




private slots:
    void add();
    void rem();
    void loadPeriod(QModelIndex x);
    void savePeriod();




    void addPoint();
    void remPoint();
    void loadAllPerSelectedPoints();






};

#endif // WINNORMA_H

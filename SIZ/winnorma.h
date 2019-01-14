#ifndef WINNORMA_H
#define WINNORMA_H

#include <QDialog>
#include <QScopedPointer>
#include <QSqlRelationalTableModel>
#include <QSqlQuery>

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

    QScopedPointer<QSqlTableModel> allUnits;
    QScopedPointer<QSqlTableModel> allSeats;
    QScopedPointer<QSqlTableModel> selectedPoints;
    QScopedPointer<QSqlRelationalTableModel> selectedPointsShowNames;





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

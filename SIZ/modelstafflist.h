#ifndef MODELSHTAT_H
#define MODELSHTAT_H

#include <QObject>
#include <QSqlTableModel>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QCalendarWidget>
#include <QDateEdit>
#include <QDebug>
#include <QModelIndex>
#include <QDate>
#include <QDateTime>
#include <QCheckBox>


class delegateStaffList : public QStyledItemDelegate
{
    Q_OBJECT
public:
    delegateStaffList(QObject *parent = nullptr);
    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
private:
    QList<QSqlTableModel*> columns;
};



class modelStaffList : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit modelStaffList(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // MODELSHTAT_H

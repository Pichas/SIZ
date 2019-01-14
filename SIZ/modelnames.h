#ifndef MODELNAIMENOVANIE_H
#define MODELNAIMENOVANIE_H

#include <QObject>
#include <QSqlTableModel>
#include <QStyledItemDelegate>
#include <QComboBox>



class delegateNames : public QStyledItemDelegate
{
    Q_OBJECT
public:
    delegateNames(QObject *parent = nullptr);
    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
private:
    QList<QSqlTableModel*> columns;
};

class modelNames : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit modelNames(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // MODELNAIMENOVANIE_H

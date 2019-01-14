#include "modelnames.h"

delegateNames::delegateNames(QObject *parent) : QStyledItemDelegate(parent)
{
    QStringList tbls; //имена таблиц для подстановок

    tbls << "" //ИД
         << "" //Наименование
         << "_ед_изм" //Еденицы измерения
         << "_признак" //признак
         << "_тип" //тип
         << ""; //сертификат

    for (int i = 0 ; i < tbls.count() ; i++){
        if (tbls.at(i).isEmpty()) {
            columns << nullptr; //пропустить столбец для ручного ввода
            continue;
        }

        columns << new QSqlTableModel(this, QSqlDatabase::database("mainBase"));
        columns.last()->setTable(tbls.at(i));
        columns.last()->select();
    }
}

QWidget *delegateNames::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //выбор только из доступных вариатов
    if(columns[index.column()]){
        QComboBox* cb = new QComboBox(parent);
        cb->setModel(columns[index.column()]);
        return cb;
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void delegateNames::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{

    QStyledItemDelegate::setModelData(editor, model, index);
}

void delegateNames::setEditorData(QWidget *editor, const QModelIndex &index) const
{

    QStyledItemDelegate::setEditorData(editor, index);
}


modelNames::modelNames(QObject *parent, QSqlDatabase db) : QSqlTableModel(parent, db)
{
}

QVariant modelNames::data(const QModelIndex &index, int role) const
{

    return QSqlTableModel::data(index, role);
}



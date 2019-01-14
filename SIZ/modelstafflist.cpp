#include "modelstafflist.h"

delegateStaffList::delegateStaffList(QObject *parent)
    : QStyledItemDelegate(parent)
{
    QStringList tbls; //имена таблиц для подстановок

    tbls << "" //таб номер
         << "" //фамилия
         << "" //имя
         << "" //отчество
         << "цех" //цех
         << "должности" //должность
         << "" //дата поступления
         << "" // дата перевода
         << "_пол" //пол
         << "_рост" //рост
         << "_одежда" //размер одежды
         << "_обувь" //размер обуви
         << "_головной_убор" //размер головы
         << "_противогаз" //размер противогаза
         << "_респиратор" //размер респиратора
         << "_рукавицы" //размер рукавиц
         << "_перчатки" //размер первчаток
         << "_цвет" //цвет каски
         << ""; //декрет


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

QWidget *delegateStaffList::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //выбор только из доступных вариатов
    if(columns[index.column()]){
        QComboBox* cb = new QComboBox(parent);
        cb->setModel(columns[index.column()]);
        return cb;
    }


    if (index.column() == 6 || index.column() == 7){ //дата трудоустройства и перевода на новую должность
        QDateEdit* c = new QDateEdit(parent);
        c->setCalendarPopup(true);
        return c;
    }

    if (index.column() == 18){ //декрет
        QCheckBox* chb = new QCheckBox(parent);
        return  chb;
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void delegateStaffList::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (index.column() == 18){
        model->setData(index, qobject_cast<QCheckBox*>(editor)->isChecked(), Qt::EditRole);
        return;
    }

    QStyledItemDelegate::setModelData(editor, model, index);
}

void delegateStaffList::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (index.column() == 18){
        qobject_cast<QCheckBox*>(editor)->setChecked(index.data(Qt::EditRole).toBool());
        return;
    }

    QStyledItemDelegate::setEditorData(editor, index);
}


modelStaffList::modelStaffList(QObject *parent, QSqlDatabase db) : QSqlTableModel(parent, db)
{

}

QVariant modelStaffList::data(const QModelIndex &index, int role) const
{
    if (index.column() == 6 || index.column() == 7){
        switch (role) {
        case Qt::DisplayRole:
            return index.data(Qt::EditRole).toDate();
        }
    }

    if (index.column() == 18){
        switch (role) {
        case Qt::DisplayRole:
            return "";
            break;
        case Qt::CheckStateRole:
            return index.data(Qt::EditRole).toBool();
            break;


        }
    }

    return QSqlTableModel::data(index, role);
}



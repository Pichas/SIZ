#ifndef MYCELL_H
#define MYCELL_H

#include <QObject>
#include <QVariant>
#include <QColor>
#include <QAxObject>
#include <QDebug>



class myCell : public QObject
{
    Q_OBJECT
public:
    explicit myCell(QVariant val = 0, QObject *parent = nullptr) : QObject(parent), value(val){}
    ~myCell(){}

    //value
    void setValue(QVariant v){value = v;}


    template<typename T>
    T getValue() const {
        return qvariant_cast<T>(value);
    }

    QVariant getValue() const {
            return value;
    }

private:
    QVariant value;

signals:

public slots:
};




#endif // MYCELL_H

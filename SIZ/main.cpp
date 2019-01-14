#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QSharedMemory>
#include <QMessageBox>
#include <QtSql>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QFileDialog>


#include "myset.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSharedMemory shmem("SIZ_UGRES_Pich_SV_01012019"); //запрет запуска двух экземпляров программ
    if (shmem.attach())
    {
        QMessageBox::warning(nullptr,"Ошибка","Программа уже запущена");
        return 1;
    }
    else
    {

        SETTING_SET_PROGNAME = "SIZ";
        QString pathToDB = INI->value("dbFile", "").toString();

        if (pathToDB.isEmpty()){
            QString pathToDB = QFileDialog::getOpenFileName(nullptr,
                                                            "Открыть файл базы данных",
                                                            "",
                                                            "Microsoft Access (*.mdb, *.accdb)");
            if (!pathToDB.isEmpty())
                INI->setValue("dbFile", pathToDB);
            INI->sync();
            return 1;
        }


        QSqlDatabase db = QSqlDatabase::addDatabase("QODBC","mainBase");
        db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=" + pathToDB);


        QSplashScreen* splash = new QSplashScreen(QPixmap("://src/logo.jpg"));
        splash->show();

        splash->showMessage("Подключение к БД ...", Qt::AlignCenter | Qt::AlignBottom, Qt::black);
        QThread::msleep(500);
        if(db.open()){
            splash->showMessage("Подключние к БД выполнено", Qt::AlignCenter | Qt::AlignBottom, Qt::black);
            QThread::msleep(200);
        }else {
            splash->showMessage("Ошибка подключения к БД.\r\n" + db.lastError().text(), Qt::AlignCenter | Qt::AlignBottom, Qt::black);
            QThread::sleep(5);
            qDebug() << QSqlDatabase::drivers();
            qDebug() << db.lastError().text();

            return 2;
        }




        MainWindow w;


        splash->finish(&w);
        w.show();

        return a.exec();
    }
}

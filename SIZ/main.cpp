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
#include "version.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);


    QFile f("version");
    if(f.open(QIODevice::WriteOnly)){   //write cur version
        f.write(VER_FILEVERSION_STR);
        f.flush();
        f.close();
    }
    QThread::msleep(100);


    QSharedMemory shmem("SIZ_UGRES_Pich_SV_01012019"); //запрет запуска двух экземпляров программ
    if (shmem.attach())
    {
        QMessageBox::warning(nullptr,"Ошибка","Программа уже запущена");
        return 1;
    }
    else
    {
        shmem.create(1);
        SETTING_SET_PROGNAME = "SIZ";

        QSplashScreen* splash = new QSplashScreen(QPixmap("://src/logo.jpg"));
        splash->show();

#ifndef ALL //если все филиалы, то пропустить открытие первой базы

        QString pathToDB = INI->value("dbFile", "").toString();

        if (pathToDB.isEmpty() || !QFile::exists(pathToDB)){ //если файл не указан или его не существует, то запросить новое расположение базы
            QString pathToDB = QFileDialog::getOpenFileName(nullptr,
                                                            "Открыть файл базы данных",
                                                            "",
                                                            "Microsoft Access (*.mdb, *.accdb)");
            if (!pathToDB.isEmpty()){
                INI->setValue("dbFile", pathToDB); //вписать в конфиг новое расположение
                INI->sync();

                QProcess::startDetached(argv[0], QStringList()); //перезапустить программу
            }
            return 1;
        }


        QSqlDatabase db = QSqlDatabase::addDatabase("QODBC","mainBase");
        db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=" + pathToDB);

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

#endif

        MainWindow w;

        splash->finish(&w);
        w.show();

        return a.exec();
    }
}

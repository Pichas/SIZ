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
        
        
        if (!QFile("./tmpl/result.xltm").exists()){
            splash->showMessage("ОШИБКА! Файл result.xltm отсутствует.\nДальнейшая работа программы невозможна.", Qt::AlignCenter | Qt::AlignBottom, Qt::black);
            QThread::sleep(10);
            return 3;
        }
        
        if (!QFile("./tmpl/emplCard.xltm").exists()){
            splash->showMessage("ОШИБКА! Файл emplCard.xltm отсутствует.\nДальнейшая работа программы невозможна.", Qt::AlignCenter | Qt::AlignBottom, Qt::black);
            QThread::sleep(10);
            return 3;
        }
        
        if (!QFile("./tmpl/stock.xlsx").exists()){
            splash->showMessage("ОШИБКА! Файл stock.xlsx отсутствует.\nДальнейшая работа программы невозможна.", Qt::AlignCenter | Qt::AlignBottom, Qt::black);
            QThread::sleep(10);
            return 3;
        }

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
        
        QSqlQuery q("", QSqlDatabase::database("mainBase"));
        q.exec("SELECT * FROM _ПЕ;");
        if (!(q.first() && q.value(0).toString() == "-")){
            QMessageBox::StandardButton result = QMessageBox::question(nullptr,"Внимание","Вы используете устаревшую базу данных.\nОбновить?\nВНИМАНИЕ: будет создана резервная копия БД рядом с существующей");
            if(result == QMessageBox::No) return 5;
            
            
            splash->showMessage("Создание резервной копии\n" + pathToDB + QDateTime::currentDateTime().toString("__yyyy_MM_dd___hh_mm") + ".bak", Qt::AlignCenter | Qt::AlignBottom, Qt::black);
            QThread::sleep(5);
            
            if (!QFile(pathToDB).copy(pathToDB + QDateTime::currentDateTime().toString("__yyyy_MM_dd___hh_mm") + ".bak")){
                splash->showMessage("ОШИБКА при создании резервной копии", Qt::AlignCenter | Qt::AlignBottom, Qt::black);
                QThread::sleep(10);
                return 3;
            }
            
            splash->showMessage("Создание таблиц", Qt::AlignCenter | Qt::AlignBottom, Qt::black);
            QThread::sleep(2);
            
            q.exec("create table _ПЕ  (ПЕ text(255)  Constraint  PrimaryKey  PRIMARY KEY);");
            if (!q.lastError().text().isEmpty()){
                splash->showMessage("ОШИБКА\n" + q.lastError().text(), Qt::AlignCenter | Qt::AlignBottom, Qt::black);
                QThread::sleep(2);
            }
            
            q.exec("create table _НГ  (НГ int  Constraint  PrimaryKey  PRIMARY KEY);");
            if (!q.lastError().text().isEmpty()){
                splash->showMessage("ОШИБКА\n" + q.lastError().text(), Qt::AlignCenter | Qt::AlignBottom, Qt::black);
                QThread::sleep(2);
            }
            
            q.exec(" create table категории (Код int  Constraint  PrimaryKey  PRIMARY KEY, Цех text(255) REFERENCES цех(Сокр), Должность text(255)  REFERENCES должности(Должность), "
                   " ПЕ text(255)  REFERENCES _ПЕ(ПЕ), НГ int   REFERENCES _НГ(НГ));");
            if (!q.lastError().text().isEmpty()){
                splash->showMessage("ОШИБКА\n" + q.lastError().text(), Qt::AlignCenter | Qt::AlignBottom, Qt::black);
                QThread::sleep(2);
            }
            
            
            
            splash->showMessage("Заполнение таблиц", Qt::AlignCenter | Qt::AlignBottom, Qt::black);
            QThread::sleep(2);
            
            
            q.exec("INSERT INTO _ПЕ VALUES ('-')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ_АУП')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ_Прочие')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ_Служ_КТЦ')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ_Служ_ОТД')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ_Служ_ОЭ')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ_Служ_ХЦ')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ_Служ_ЦАСУТП')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ_Служ_ЦТиИК')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ_Служ_ЦХЛ')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ_Служ_ЦЦР')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ_Служ_ЭЦ')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ1')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ2')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ3')");
            q.exec("INSERT INTO _ПЕ VALUES ('ПЕ4')");
            
            
            q.exec("INSERT INTO _НГ VALUES (0)");
            q.exec("INSERT INTO _НГ VALUES ('112000')");
            q.exec("INSERT INTO _НГ VALUES ('114000')");
            q.exec("INSERT INTO _НГ VALUES ('117060')");
            q.exec("INSERT INTO _НГ VALUES ('119000')");
            q.exec("INSERT INTO _НГ VALUES ('211210')");
            q.exec("INSERT INTO _НГ VALUES ('1150006')");
            
        }
        
        MainWindow w;

        splash->finish(&w);
        w.show();

        return a.exec();
    }
}

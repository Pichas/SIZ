#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pbStaffList, &QPushButton::clicked, [&](bool /*b*/){
        QScopedPointer <winStaffList> win(new winStaffList(nullptr));
        win->exec();});

    connect(ui->pbNames, &QPushButton::clicked, [&](bool /*b*/){
        QScopedPointer <winNames> win(new winNames(nullptr));
        win->exec();});

    connect(ui->pbNorm, &QPushButton::clicked, [&](bool /*b*/){
        QScopedPointer <winNorma> win(new winNorma(nullptr));
        win->exec();});

    connect(ui->pbStock, &QPushButton::clicked, [&](bool /*b*/){
        QScopedPointer <winStock> win(new winStock(nullptr));
        win->exec();});

    connect(ui->pbEmplCard, &QPushButton::clicked, [&](bool /*b*/){
        QScopedPointer <winEmplCard> win(new winEmplCard(nullptr));
        win->exec();});

    connect(ui->pbReport, &QPushButton::clicked, [&](bool /*b*/){
        QScopedPointer <winReport> win(new winReport(nullptr));
        win->exec();});

    QTimer::singleShot(10 * 1000, this, &MainWindow::checkUpdate);//проверить обновление через 10 сек



#ifdef ALL

    //загрузить список станций
    for (int i = 0; i < INI->allKeys().count(); i++) {
        if (INI->allKeys().at(i)[0] == '_')
            ui->cbSelectDB->addItem(INI->allKeys().at(i).mid(1)); //удалить нижний прочерк в начале
    }


    connect(ui->pbEditDBList, &QPushButton::clicked, [&](bool /*b*/){
        QScopedPointer <winDBList> win(new winDBList(nullptr));
        win->exec();});

    connect(ui->cbSelectDB, &QComboBox::currentTextChanged, [&](QString newVal){
        QSqlDatabase db = QSqlDatabase::database("mainBase");
        if (db.isValid()) db.close();


        db = QSqlDatabase::addDatabase("QODBC","mainBase");
        db.setDatabaseName("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=" + INI->value("_" + newVal, "").toString());

        bool isEnabled;
        if (!db.open()){
            QMessageBox::critical(nullptr, "Ошибка","Невозможно открыть базу данных\r\n" + db.lastError().text());
            isEnabled = false;
        }else{
            isEnabled = true;
        }

        ui->pbStaffList->setEnabled(isEnabled);
        ui->pbNames->setEnabled(isEnabled);
        ui->pbNorm->setEnabled(isEnabled);
        ui->pbStock->setEnabled(isEnabled);
        ui->pbEmplCard->setEnabled(isEnabled);
        ui->pbReport->setEnabled(isEnabled);
    });


    ui->cbSelectDB->currentTextChanged(ui->cbSelectDB->currentText());

#else

    ui->pbEditDBList->hide();
    ui->cbSelectDB->hide();

#endif

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::checkUpdate()
{
    appUpdate* upd = new appUpdate();
    connect(upd, &appUpdate::needUpdate, [&]{
        QMessageBox::StandardButton reply = QMessageBox::information(nullptr, "Обновление",
                                        "Доступно обновление.\nОбновить?",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        switch (reply) {
        case QMessageBox::Yes:
            QProcess::startDetached("update.exe", QStringList(), QCoreApplication::applicationDirPath());
            exit(2);
            break;
        default:
            break;
        }
    });
}

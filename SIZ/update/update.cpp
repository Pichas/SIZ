#include "update.h"


update::update(QSplashScreen *spl) : working(true), splScr(spl)
{
    splScr->showMessage("Проверка наличия обновлений", Qt::AlignCenter | Qt::AlignBottom, Qt::black);

    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadFinished(QNetworkReply*)));
    connect(this, &update::sFinish, [&]{working = false;});

    //подготовка машины обновлений
    mUrl.setScheme("https");
    mUrl.setHost("raw.githubusercontent.com");
    mUrl.setPort(443);

    QState *sInit           = new QState(&updateMachine);
    QState *sCheckVersion   = new QState(&updateMachine);
    QState *sUpdate         = new QState(&updateMachine);

    sInit->addTransition            (this, SIGNAL(sNextState()), sCheckVersion);
    sCheckVersion->addTransition    (this, SIGNAL(sNextState()), sUpdate);

    connect(sInit,          SIGNAL(entered()), SLOT(mInit()));
    connect(sCheckVersion,  SIGNAL(entered()), SLOT(mCheckVersion()));
    connect(sUpdate,        SIGNAL(entered()), SLOT(mUpdate()));

    updateMachine.setInitialState(sInit);
    updateMachine.start();
}

update::~update()
{
    updateMachine.stop();
    this->deleteLater();
}



void update::mInit()
{
    //проверка доступности сервиса
    splScr->showMessage("Проверка связи с сервером", Qt::AlignCenter | Qt::AlignBottom, Qt::black);
    QTcpSocket s;
    s.connectToHost(mUrl.host(), static_cast<uint16_t>(mUrl.port()), QIODevice::ReadOnly);


    if(s.waitForConnected(500)) {
        qDebug() << "OK";
        s.disconnectFromHost();
        emit sNextState();
    } else {
        splScr->showMessage("Нет связи с сервером", Qt::AlignCenter | Qt::AlignBottom, Qt::black);

        QMessageBox::StandardButton reply;
        reply = QMessageBox::critical(nullptr, "Ошибка связи",
                                        "Нет связи с сервером. Проверьте соединение с интернетом и повторите попытку.\n" + s.errorString(),
                                        QMessageBox::Abort | QMessageBox::Ignore);

        switch (reply) {
        case QMessageBox::Ignore:
            emit sFinish();
            break;
        case QMessageBox::Abort:
            exit(1);
        default:
            break;
        }
    }
}

void update::mCheckVersion()
{
    splScr->showMessage("Проверка наличия обновлений", Qt::AlignCenter | Qt::AlignBottom, Qt::black);

    curState = state::check;
    curFileToCheck = QCoreApplication::applicationDirPath() + "/version";
    //NOTE version
    QUrl version(mUrl);
    version.setPath("/Pichas/SIZ/master/bin/version");

    doDownload(version);
}

void update::mUpdate()
{
    splScr->showMessage("Скачивание обновлений", Qt::AlignCenter | Qt::AlignBottom, Qt::black);

    curState = state::download;
    curFileToCheck = QCoreApplication::applicationDirPath() + "/update.exe";
    //NOTE updater
    QUrl update(mUrl);
    update.setPath("/Pichas/SIZ/master/bin/update");

    doDownload(update);
}


void update::doDownload(const QUrl &url)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);
    //добавить в загрузки, чтобы не потерять ответ
    currentDownloads.append(reply);
}


void update::downloadFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    qDebug() << url;

    if (reply->error()) {
        splScr->showMessage(QString("Ошибка при скачивании %1.\r\n%2").arg(url.toEncoded().constData(),reply->errorString()),
                            Qt::AlignCenter | Qt::AlignBottom, Qt::black);
        QThread::sleep(1);
        reply->deleteLater();
        return;
    }

    QFile f(curFileToCheck);

    switch (curState) {
    case state::check:
        if (f.exists() && f.open(QFile::ReadOnly)){
            QCryptographicHash hFile(QCryptographicHash::Md5);
            hFile.addData(f.readAll());
            qDebug() << hFile.result();

            QCryptographicHash hReply(QCryptographicHash::Md5);
            hReply.addData(reply->readAll());
            qDebug() << hReply.result();

            if(hFile.result() == hReply.result()){
                emit sFinish();
            } else {
                emit sNextState();
            }

        }else{
            qDebug() << reply->readAll();
            emit sNextState();
        }

        currentDownloads.removeAll(reply);

        break;
    case state::download:
        if (f.open(QIODevice::WriteOnly)) {
            f.write(reply->readAll());
            f.flush();
            f.close();
        }

        currentDownloads.removeAll(reply);
        if (currentDownloads.isEmpty()){
            QProcess::startDetached("update.exe", QStringList(), QCoreApplication::applicationDirPath());
            exit(2);
        }
        break;
    }
    reply->deleteLater();
}













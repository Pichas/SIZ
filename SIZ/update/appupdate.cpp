#include "appupdate.h"


appUpdate::appUpdate() : working(true)
{
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadFinished(QNetworkReply*)));
    connect(this, &appUpdate::sFinish, [&]{
        working = false;
        updateMachine.stop();
        this->deleteLater();
    });

    //подготовка машины обновлений
    mUrl.setScheme("https");
    mUrl.setHost("raw.githubusercontent.com");
    mUrl.setPort(443);

    sInit           = new QState(&updateMachine);
    sCheckVersion   = new QState(&updateMachine);
    sUpdate         = new QState(&updateMachine);

    sInit->addTransition            (this, SIGNAL(sNextState()), sCheckVersion);
    sInit->addTransition            (this, SIGNAL(sRetry()), sInit);

    sCheckVersion->addTransition    (this, SIGNAL(sNextState()), sUpdate);

    connect(sInit,          SIGNAL(entered()), SLOT(mInit()));
    connect(sCheckVersion,  SIGNAL(entered()), SLOT(mCheckVersion()));
    connect(sUpdate,        SIGNAL(entered()), SLOT(mUpdate()));

    updateMachine.setInitialState(sInit);
    updateMachine.start();

    qDebug() << "start";
}

appUpdate::~appUpdate()
{
    for (QNetworkReply* reply : currentDownloads){
        reply->deleteLater();
    }

    sInit->deleteLater();
    sCheckVersion->deleteLater();
    sUpdate->deleteLater();

    qDebug() << "stop updater";
}



void appUpdate::mInit()
{
    static int rets = 3;
    //проверка доступности сервиса
    QTcpSocket s;
    s.connectToHost(mUrl.host(), static_cast<uint16_t>(mUrl.port()), QIODevice::ReadOnly);

    if(s.waitForConnected(500)) {
        qDebug() << "connetction ok";
        emit sendMessage("Соединение с сервером установлено");
        s.disconnectFromHost();
        emit sNextState();
    } else {
        rets--;
        qDebug() << "connetction error";
        emit sendMessage("Ошибка соединения с сервером обновлений");
        QTimer::singleShot(10 * 1000, this, &appUpdate::sRetry); //через 10 сек повторить
    }
    if(!rets){
        emit sFinish();
    }
}

void appUpdate::mCheckVersion()
{
    curState = state::check;
    curFileToCheck = QCoreApplication::applicationDirPath() + "/version";
    //NOTE version
    QUrl version(mUrl);
    version.setPath("/Pichas/SIZ/master/bin/version");

    emit sendMessage("Проверка наличия обновлений");
    doDownload(version);
}

void appUpdate::mUpdate()
{
    curState = state::download;
    curFileToCheck = QCoreApplication::applicationDirPath() + "/update.exe";
    //NOTE updater
    QUrl update(mUrl);

#ifdef __i386
    update.setPath("/Pichas/SIZ/master/bin/update_x86");
#elif __x86_64__
    update.setPath("/Pichas/SIZ/master/bin/update_x64");
#elif //не поддерживается архитектура
    emit sFinish();
    return;
#endif

    emit sendMessage("Скачивание обновлений");
    doDownload(update);
}


void appUpdate::doDownload(const QUrl &url)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);
    //добавить в загрузки, чтобы не потерять ответ
    currentDownloads.append(reply);
}


void appUpdate::downloadFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    qDebug() << url;

    if (reply->error()) {
        QThread::sleep(1);
        emit sFinish();
        return;
    }

    QFile f(curFileToCheck);

    switch (curState) {
    case state::check:
        if (f.exists() && f.open(QFile::ReadOnly)){
            QStringList localParts = QString (f.readAll()).split('.');
            QStringList remoteParts = QString (reply->readAll()).split('.');

            for (int i = 0; i < qMin(remoteParts.size(), localParts.size()); ++i) {
                if (remoteParts[i].toInt() > localParts[i].toInt()){
                    emit sNextState();
                    return;
                }
            }
            emit sFinish();

        }else{
            emit sNextState();
        }
        break;

    case state::download:
        if (f.open(QIODevice::WriteOnly)) {
            f.write(reply->readAll());
            f.flush();
            f.close();
            emit needUpdate();
        }

        emit sFinish();
        break;
    }
}













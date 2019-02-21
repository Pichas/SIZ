#ifndef UPDATE_H
#define UPDATE_H

#include <QProcess>
#include <QFile>
#include <QDir>
#include <QApplication>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QtNetwork>
#include <QTemporaryFile>
#include <QDialog>
#include <QTimer>
#include <QTcpSocket>
#include <QStateMachine>
#include <QCryptographicHash>
#include <QMessageBox>





class appUpdate : public QObject
{

    Q_OBJECT

    enum state{
        check,
        download
    };

public:
    appUpdate();
    bool waiteForFinish() const {return working;}
    ~appUpdate();

public slots:


private:
    bool working;

    state curState;
    QStateMachine updateMachine;
    QState *sInit;
    QState *sCheckVersion;
    QState *sUpdate;



    QUrl mUrl;
    QString curFileToCheck;

    QNetworkAccessManager manager;
    QList<QNetworkReply *> currentDownloads;


    void doDownload(const QUrl &url);

private slots:
    void downloadFinished(QNetworkReply *reply);

    //machine
    void mInit();
    void mCheckVersion();
    void mUpdate();

signals:
    void sRetry();
    void sNextState();
    void sFinish();
    void needUpdate();
};







#endif // UPDATE_H

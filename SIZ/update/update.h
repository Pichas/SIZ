#ifndef UPDATE_H
#define UPDATE_H

#include <QSettings>
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
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QTcpSocket>
#include <QStateMachine>
#include <QSplashScreen>
#include <QCryptographicHash>
#include <QMessageBox>




class update : public QObject
{

    Q_OBJECT

    enum state{
        check,
        download
    };

public:
    update(QSplashScreen* spl);
    bool waiteForFinish() const {return working;}
    ~update();

public slots:


private:
    bool working;
    QSplashScreen* splScr;

    state curState;
    QStateMachine updateMachine;
    QString curFileToCheck;

    QUrl mUrl;
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
    void sNextState();
    void sFinish();
};







#endif // UPDATE_H

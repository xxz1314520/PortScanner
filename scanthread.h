#ifndef SCANTHREAD_H
#define SCANTHREAD_H

#include <QThread>
#include <QString>
#include <QCoreApplication>
#include <QDebug>
#include <QByteArray>
#include <QHostAddress>
#include <QNetworkDatagram>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QList>
#include <QMutex>
#include <QWaitCondition>


class ScanThread : public QThread {
    Q_OBJECT

public:
    ScanThread(const QString& ip, int startPort, int endPort, const QString& type,int id);

    int id;
    bool need_reset_progress_value = false;


    void setValue(int newValue);
    void setKeyAvailable(bool available);
public slots:


signals:
    void scanResult(int port, const QString& result);
    void icmpResult(const QString& result); // 新增的信号，仅用于 ICMP 扫描
    void removeMe(ScanThread*);
    void send_addprogressValue();

protected:
    void run() override;

private:
    QString ip;
    int startPort;
    int endPort;

    QString scanType;


    bool keyAvailable = false; // “钥匙”标志
};

#endif // SCANTHREAD_H

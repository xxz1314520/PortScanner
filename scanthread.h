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
//    void sendSynPacket(pcap_t* handle, const char* src_ip, const char* dst_ip, int dst_port);

//    ~ScanThread(){
//        progressValue--;
//    }
//    volatile bool isTerminated = false;
//    void terminate();
    int id;
    bool need_reset_progress_value = false;
//    void static reset_progress_value(){
//        progressValue = 0;
//    }
    void setValue(int newValue);
    void setKeyAvailable(bool available);
public slots:
//    void rest_reset_progress_value()
//    {
//        setValue(0);
//    }

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

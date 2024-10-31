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



class ScanThread : public QThread {
    Q_OBJECT

public:
    ScanThread(const QString& ip, int startPort, int endPort, const QString& type,int id);
//    void sendSynPacket(pcap_t* handle, const char* src_ip, const char* dst_ip, int dst_port);

    static int id_count;
signals:
    void scanResult(int port, const QString& result);
    void icmpResult(const QString& result); // 新增的信号，仅用于 ICMP 扫描

protected:
    void run() override;

private:
    QString ip;
    int startPort;
    int endPort;
    int id;
    QString scanType;
};

#endif // SCANTHREAD_H

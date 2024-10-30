#ifndef SCANTHREAD_H
#define SCANTHREAD_H

#include <QThread>
#include <QString>

class ScanThread : public QThread {
    Q_OBJECT

public:
    ScanThread(const QString& ip, int port, const QString& type);

signals:
    void scanResult(int port, const QString& result);
    void icmpResult(const QString& result); // 新增的信号，仅用于 ICMP 扫描

protected:
    void run() override;

private:
    QString ip;
    int port;
    QString scanType;
};

#endif // SCANTHREAD_H

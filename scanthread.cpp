#include "scanthread.h"
#include <QTcpSocket>
#include <QUdpSocket>
#include <QProcess>
#include <QOperatingSystemVersion>

ScanThread::ScanThread(const QString& ip, int port, const QString& type)
    : ip(ip), port(port), scanType(type) {}

void ScanThread::run() {
    try {
        if (scanType == "ICMP") {
            // ICMP 扫描（Ping），不使用端口
            QProcess process;
            QStringList arguments;

            // 判断系统平台，并设置合适的 ping 参数
#ifdef Q_OS_WIN
            arguments << "-n" << "1" << ip;
#else
            arguments << "-c" << "1" << ip;
#endif

            process.start("ping", arguments);
            bool finished = process.waitForFinished(3000);  // 设置3秒的等待时间

            QString result;
            if (finished) {
                QString output = process.readAllStandardOutput();
                if (output.contains("TTL=") || output.contains("ttl=")) {
                    result = QString("%1 is online").arg(ip);
                } else {
                    result = QString("%1 is offline").arg(ip);
                }
            } else {
                result = QString("%1 is offline (timeout)").arg(ip);
            }
            emit icmpResult(result);  // 发出 ICMP 结果信号
        } else if (scanType == "TCP") {
            QString result;
            QTcpSocket socket;
            socket.connectToHost(ip, port);
            if (socket.waitForConnected(1000)) {
                result = QString("TCP Port %1 is open").arg(port);
                socket.close();
            } else {
                result = QString("TCP Port %1 is closed").arg(port);
            }
            emit scanResult(port, result);
        } else if (scanType == "UDP") {
            QString result;
            QUdpSocket socket;
            socket.writeDatagram("test", QHostAddress(ip), port);
            if (socket.waitForReadyRead(1000)) {
                result = QString("UDP Port %1 is open").arg(port);
            } else {
                result = QString("UDP Port %1 is closed or filtered").arg(port);
            }
            emit scanResult(port, result);
        }
    } catch (...) {
        emit scanResult(port, QString("Error scanning port %1").arg(port));
    }
}

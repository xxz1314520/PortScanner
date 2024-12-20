#include "scanthread.h"
#include <QTcpSocket>
#include <QUdpSocket>
#include <QProcess>
#include <QOperatingSystemVersion>


ScanThread::ScanThread(const QString& ip, int startPort, int endPort, const QString& type, int id)
    : ip(ip), startPort(startPort),endPort(endPort), scanType(type) {
    this->id = id;

}


void ScanThread::run() {
    // 执行任务
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
            emit send_addprogressValue();
        } else {
            // 处理 TCP、UDP、TCP SYN 扫描，遍历分配的端口范围
            for (int port = startPort; port <= endPort && !QThread::currentThread()->isInterruptionRequested(); ++port) {
                QString result;

                if (scanType == "TCP") {
                    QTcpSocket socket;
                    socket.connectToHost(ip, port);
                    if (socket.waitForConnected(1000)) {
                        result = "open";
                        socket.close();
                    } else {
                        result = "close";
                    }
                    emit scanResult(port, result);
                    emit send_addprogressValue();
                } else if (scanType == "UDP") {
                    QUdpSocket socket;
                    socket.writeDatagram("test", QHostAddress(ip), port);
                    if (socket.waitForReadyRead(1000)) {
                        result = "open";
                    } else {
                        result = "close";
                    }
                    emit scanResult(port, result);
                    emit send_addprogressValue();
                } else if (scanType == "TCP SYN") {

                }
            }


        }

    } catch (...) {
        emit scanResult(startPort, QString("Error scanning range %1-%2").arg(startPort).arg(endPort));
    }
}








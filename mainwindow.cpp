#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), pendingTasks(0) {
    ui->setupUi(this);
    thread_count = 0;

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_startButton_clicked() {
    ui->resultListWidget->clear();
    //    scanResults.clear();  // 清空上次扫描的结果
    scanResults2.clear();

    QString ip = ui->ipLineEdit->text();
    if (ip.isEmpty()) {
        ui->resultListWidget->addItem("请输入有效的IP地址");
        return;
    }

    QString scanType = ui->scanTypeComboBox->currentText();

    if (scanType == "ICMP") {
        // ICMP 扫描，不需要端口范围
        ScanThread* thread = new ScanThread(ip, 0,0, scanType,thread_count);

        connect(thread, &ScanThread::icmpResult, this, &MainWindow::handleICMPResult);
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);

        thread->start();
        return;
    }

    // 如果是 TCP 或 UDP 扫描，处理端口范围或单个端口
    QString portRange = ui->portRangeLineEdit->text();

    int startPort, endPort;
    if (portRange.contains("-")) {
        // 解析端口范围
        QStringList range = portRange.split("-");
        if (range.size() != 2) {
            ui->resultListWidget->addItem("端口范围格式错误，请输入如 80-100");
            return;
        }

        startPort = range[0].toInt();
        endPort = range[1].toInt();

        if (startPort < 0 || endPort > 65535 || startPort > endPort) {
            ui->resultListWidget->addItem("端口范围无效，请输入 0-65535 范围内的端口");
            return;
        }
    } else {
        // 解析单个端口
        startPort = endPort = portRange.toInt();
        if (startPort < 0 || startPort > 65535) {
            ui->resultListWidget->addItem("端口无效，请输入 0-65535 范围内的端口");
            return;
        }
    }

    pendingTasks = endPort - startPort + 1;  // 初始化任务计数

    int totalPorts = endPort - startPort + 1;
    int portsPerThread = totalPorts / max_thread;    // 每个线程处理的端口数量
    int extraPorts = totalPorts % max_thread;        // 多余的端口分配给前几个线程

    int currentStartPort = startPort;

    for (int i = 0; i < max_thread; ++i) {
        int currentEndPort = currentStartPort + portsPerThread - 1;

        if (extraPorts > 0) {
            currentEndPort++;
            extraPorts--;
        }

        if (currentStartPort <= endPort) {
            ScanThread* thread = new ScanThread(ip, currentStartPort, currentEndPort, scanType, thread_count++);
            connect(thread, &ScanThread::scanResult, this, &MainWindow::handleScanResult);
            connect(thread, &QThread::finished, thread, &QObject::deleteLater);

            thread->start();
        }

        currentStartPort = currentEndPort + 1;
    }



//    for (int port = startPort; port <= endPort; ++port) {
//        ScanThread* thread = new ScanThread(ip, port, scanType);

//        connect(thread, &ScanThread::scanResult, this, &MainWindow::handleScanResult);
//        connect(thread, &QThread::finished, thread, &QObject::deleteLater);

//        thread->start();
//    }
}

// 处理 TCP 和 UDP 扫描结果
void MainWindow::handleScanResult(int port, const QString& result) {
    // 将结果存入 QMap，以便按端口号排序
    //    scanResults[port] = result;
    scanResults2[result].append(port);
    pendingTasks--;

    // 当所有扫描任务完成后，按端口顺序输出结果
    if (pendingTasks == 0) {
        //        for (auto it = scanResults.begin(); it != scanResults.end(); ++it) {
        //            ui->resultListWidget->addItem(it.value());
        //        }
        int i = 0;
        if (scanResults2["open"].length() == 0){
            ui->resultListWidget->addItem(QString("目标主机所扫描的所有端口都未开放！"));
        }
        else{
            ui->resultListWidget->addItem(QString("针对扫描范围，目标主机开放的端口:"));
            for (i = 0; i < scanResults2["open"].length(); i++){
                ui->resultListWidget->addItem(QString::number(scanResults2["open"][i]));
            }
        }

    }
}

// 处理 ICMP 扫描结果，直接显示
void MainWindow::handleICMPResult(const QString& result) {
    ui->resultListWidget->addItem(result);
}

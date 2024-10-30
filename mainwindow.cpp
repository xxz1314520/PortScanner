#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), pendingTasks(0) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_startButton_clicked() {
    ui->resultListWidget->clear();
    scanResults.clear();  // 清空上次扫描的结果

    QString ip = ui->ipLineEdit->text();
    if (ip.isEmpty()) {
        ui->resultListWidget->addItem("请输入有效的IP地址");
        return;
    }

    QString scanType = ui->scanTypeComboBox->currentText();

    if (scanType == "ICMP") {
        // ICMP 扫描，不需要端口范围
        ScanThread* thread = new ScanThread(ip, 0, scanType);

        connect(thread, &ScanThread::icmpResult, this, &MainWindow::handleICMPResult);
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);

        thread->start();
        return;
    }

    // 如果是 TCP 或 UDP 扫描，处理端口范围
    QString portRange = ui->portRangeLineEdit->text();
    QStringList range = portRange.split("-");
    if (range.size() != 2) {
        ui->resultListWidget->addItem("端口范围格式错误，请输入如 80-100");
        return;
    }

    int startPort = range[0].toInt();
    int endPort = range[1].toInt();

    if (startPort < 0 || endPort > 65535 || startPort > endPort) {
        ui->resultListWidget->addItem("端口范围无效，请输入 0-65535 范围内的端口");
        return;
    }

    pendingTasks = endPort - startPort + 1;  // 初始化任务计数

    for (int port = startPort; port <= endPort; ++port) {
        ScanThread* thread = new ScanThread(ip, port, scanType);

        connect(thread, &ScanThread::scanResult, this, &MainWindow::handleScanResult);
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);

        thread->start();
    }
}

// 处理 TCP 和 UDP 扫描结果
void MainWindow::handleScanResult(int port, const QString& result) {
    // 将结果存入 QMap，以便按端口号排序
    scanResults[port] = result;
    pendingTasks--;

    // 当所有扫描任务完成后，按端口顺序输出结果
    if (pendingTasks == 0) {
        for (auto it = scanResults.begin(); it != scanResults.end(); ++it) {
            ui->resultListWidget->addItem(it.value());
        }
    }
}

// 处理 ICMP 扫描结果，直接显示
void MainWindow::handleICMPResult(const QString& result) {
    ui->resultListWidget->addItem(result);
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), pendingTasks(0) {
    ui->setupUi(this);
    thread_count = 0;

//    ScanThread::progress = 0;
    ui->progress_bar->setMinimum(0);
    ui->progress_bar->setMaximum(1);
    ui->progress_bar->reset();


}

MainWindow::~MainWindow() {
    delete ui;
}

QTimer *progress_timer;

void MainWindow::task_allfinished(){
    finishing_flag = true;
    thread_count = 0;
    //终止计时器
    if(progress_timer->isActive() == true)
    {
        progress_timer->stop();

    }

    for (ScanThread* thread : activeThreads) {
        if (thread != nullptr && thread->isRunning()){
            thread->requestInterruption();
            thread->wait();
        }
    }



    activeThreads.clear();
    ui->startButton->setText("开始扫描");
    isScanning = false;
    progressValue = 0;

    ui->progress_bar->reset();
    ui->horizontalSlider->setEnabled(true);
    return;
}

void MainWindow::update_progressValue(){

    progressValue = progressValue + 1;

}
void MainWindow::on_startButton_clicked() {
    ui->horizontalSlider->setDisabled(true);
    finishing_flag = false;
    thread_count = 0;
    progressValue = 0;
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
    task_num = endPort - startPort + 1;
    ui->progress_bar->setMaximum(task_num);
    ui->progress_bar->setMinimum(0);

    // 声明定时器
    progress_timer = new QTimer(this);

    ui->progress_bar->reset();

    if (isScanning) {      //需要中途停止
        task_allfinished();
        return;
    }
    else{          //初始化一次任务
        progress_timer->start(100);
        // 绑定一个匿名函数
        connect(progress_timer,&QTimer::timeout,[=]{
            // 判断是否到达了进度条的最大值
            if(progressValue != task_num)
            {
                ui->progress_bar->setValue(progressValue+1);
            }
            else
            {
                task_allfinished();
                return;
            }
        });
    }

    ui->progress_bar->reset();

    // 初始化扫描
    isScanning = true;
    ui->startButton->setText("终止扫描");
    ui->resultListWidget->clear();
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
        activeThreads.append(thread);

        connect(thread, &ScanThread::icmpResult, this, &MainWindow::handleICMPResult,Qt::QueuedConnection);
        connect(thread, &ScanThread::send_addprogressValue, this, &MainWindow::update_progressValue, Qt::QueuedConnection);
        thread->start();
        return;
    }


    int totalPorts = endPort - startPort + 1;
    int portsPerThread = totalPorts / max_thread;    // 每个线程处理的端口数量
    int extraPorts = totalPorts % max_thread;        // 多余的端口分配给前几个线程
    int currentStartPort = startPort;

    //为每个线程分配端口
    for (int i = 0; i < max_thread && i< totalPorts; ++i) {
        int currentEndPort = currentStartPort + portsPerThread - 1;

        if (extraPorts > 0) {
            currentEndPort++;
            extraPorts--;
        }

        if (currentStartPort <= endPort) {
            ScanThread* thread = new ScanThread(ip, currentStartPort, currentEndPort, scanType, thread_count++);
            activeThreads.append(thread);
            connect(thread, &ScanThread::scanResult, this, &MainWindow::handleScanResult,Qt::QueuedConnection);
            connect(thread, &ScanThread::send_addprogressValue, this, &MainWindow::update_progressValue, Qt::QueuedConnection);

            thread->start();
        }

        currentStartPort = currentEndPort + 1;
    }

}

// 处理 TCP 和 UDP 扫描结果
void MainWindow::handleScanResult(int port, const QString& result) {
    if (finishing_flag){
        return;
    }
    // 将结果存入 QMap，以便按端口号排序
    //    scanResults[port] = result;
//    scanResults2[result].append(port);

    if (pendingTasks == task_num){
        ui->resultListWidget->addItem(QString("针对扫描范围，目标主机开放的端口:"));
    }
    if(result == "open"){
        result_isnull = false;
        ui->resultListWidget->addItem(QString::number(port));
    }
    pendingTasks--;
    if (pendingTasks == 0 && result_isnull){
        ui->resultListWidget->addItem(QString("无"));
    }

}

// 处理 ICMP 扫描结果，直接显示
void MainWindow::handleICMPResult(const QString& result) {
    ui->resultListWidget->addItem(result);
    task_allfinished();
}

//线程选择
void MainWindow::on_horizontalSlider_valueChanged()
{
    max_thread = ui->horizontalSlider->value();
    ui->thread_num_label->setText(QString::number(ui->horizontalSlider->value()));
}


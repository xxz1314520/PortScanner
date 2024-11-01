#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "scanthread.h"
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

static QMutex mutex;
static int progressValue;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int max_thread = 1;
    int progressValue;
    bool result_isnull = true;
    void task_allfinished();
private slots:
    void on_startButton_clicked();
    void handleScanResult(int port, const QString& result);
    void handleICMPResult(const QString& result);
//    void removeThread(ScanThread* one);

    void update_progressValue();

    void on_horizontalSlider_valueChanged(int value);

signals:
//    void reset_progressValue();

private:
    Ui::MainWindow *ui;
    QMutex* mutex;
    // 存储端口号和对应的扫描结果
    QMap<int, QString> scanResults;
    QMap<QString, QVector<int>> scanResults2;
    int pendingTasks;  // 记录剩余的扫描任务数
    int task_num;
    int thread_count;
    bool isScanning = false;
    QVector<ScanThread*> activeThreads;
    bool finishing_flag = false;

};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "scanthread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int max_thread = 1500;
private slots:
    void on_startButton_clicked();
    void handleScanResult(int port, const QString& result);
    void handleICMPResult(const QString& result);



private:
    Ui::MainWindow *ui;

    // 存储端口号和对应的扫描结果
    QMap<int, QString> scanResults;
    QMap<QString, QVector<int>> scanResults2;
    int pendingTasks;  // 记录剩余的扫描任务数
    int thread_count;

};

#endif // MAINWINDOW_H

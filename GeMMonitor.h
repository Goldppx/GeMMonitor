//
// Created by GoldesqueMoON on 2025/12/19.
//

#ifndef UNTITLED_GEMMONITOR_H
#define UNTITLED_GEMMONITOR_H

#include <QWidget>
#include <QTimer>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

QT_BEGIN_NAMESPACE

namespace Ui {
    class GeMMonitor;
}

QT_END_NAMESPACE

class GeMMonitor : public QWidget {
    Q_OBJECT

public:
    explicit GeMMonitor(QWidget *parent = nullptr);

    ~GeMMonitor() override;

private slots:
    void updateSystemInfo(); // 更新系统信息的槽函数

private:
    Ui::GeMMonitor *ui;
    QTimer *timer; // 定时器
    class MemInfo; // 内存信息类
    class CpuInfo; // CPU信息类
    MemInfo *memInfo;
    CpuInfo *cpuInfo;
};

#endif //UNTITLED_GEMMONITOR_H
//
// Created by GoldesqueMoON on 2025/12/19.
//

// You may need to build the project (run Qt uic code generator) to get "ui_GeMMonitor.h" resolved

#include "GeMMonitor.h"
#include "cmake-build-debug/GeMMonitor_autogen/include/ui_GeMMonitor.h"

// 内存信息类的定义
class GeMMonitor::MemInfo {
    struct item {
        std::string name;
        long value{};
        std::string unit;
    };
    struct backInfo {
        item MemTotal = {"MemTotal"};
        item MemAvailable = {"MemAvailable"};
        item SwapTotal = {"SwapTotal"};
        item SwapFree = {"SwapFree"};
    } backInfo;
    bool firstTime = true;
public:
    MemInfo() = default;
    ~MemInfo() = default;
    auto getMemInfo() {
        return backInfo;
    };
    void refresh() {
        std::ifstream ifs("/proc/meminfo");
        if (!ifs.is_open()) {
            std::cerr << "Error: Failed to open /proc/meminfo" << std::endl;
            return;
        }
        item temp;
        if (firstTime) {
            while (ifs >> temp.name >> temp.value >> temp.unit) {
                if (temp.name == "MemTotal:") {backInfo.MemTotal.value = temp.value; backInfo.MemTotal.unit = temp.unit;}
                else if (temp.name == "MemAvailable:") {backInfo.MemAvailable.value = temp.value; backInfo.MemAvailable.unit = temp.unit;}
                else if (temp.name == "SwapTotal:") {backInfo.SwapTotal.value = temp.value; backInfo.SwapTotal.unit = temp.unit;}
                else if (temp.name == "SwapFree:") {backInfo.SwapFree.value = temp.value; backInfo.SwapFree.unit = temp.unit; break;}
            }
            firstTime = false;
        } else {
            while (ifs >> temp.name >> temp.value >> temp.unit) {
                if (temp.name == "MemAvailable:") {backInfo.MemAvailable.value = temp.value; backInfo.MemAvailable.unit = temp.unit;}
                else if (temp.name == "SwapFree:") {backInfo.SwapFree.value = temp.value; backInfo.SwapFree.unit = temp.unit; break;}
            }
        }
        ifs.close();
    }
};

// CPU信息类的定义
class GeMMonitor::CpuInfo {
    struct item {
        int CpuCoreNums = 0;
        int CpuThreadNums = 0;
        double CpuUsage;
    };
    struct cpuTime {
        long TotalTime;
        long IdleTime;
    };
    struct backInfo {
        item Cpu;
        cpuTime lastCpuTime;
    } backInfo{};
    bool firstTime = true;
public:
    CpuInfo() {
        backInfo.lastCpuTime = tackSnapShot();
    };
    ~CpuInfo() = default;
    [[nodiscard]] auto getCpuInfo() const {
        return backInfo;
    }
    static cpuTime tackSnapShot() {
        std::string label;
        std::ifstream stat("/proc/stat");
        cpuTime cpuTime{};
        if (!stat.is_open()) {
            std::cerr << "Error: Failed to open /proc/stat" << std::endl;
            return cpuTime;
        }
        struct snapShot {
            long user;
            long nice;
            long system;
            long idle;
            long iowait;
            long irq;
            long softirq;
            long steal;
        } s{};
        while (stat >> label >> s.user >> s.nice >> s.system >> s.idle >> s.iowait >> s.irq >> s.softirq >> s.steal) {
            if (label == "cpu") {
                long total = s.user + s.nice + s.system + s.idle + s.iowait + s.irq + s.softirq + s.steal;
                long idle = s.idle + s.iowait;
                cpuTime.TotalTime = total;
                cpuTime.IdleTime = idle;
                return cpuTime;
            }
        }
        return cpuTime;
    }
    void refresh() {
        if (firstTime) {
            std::ifstream cpuInfoFile("/proc/cpuinfo");
            if (!cpuInfoFile.is_open()) {
                std::cerr << "Error: Failed to open /proc/cpuinfo" << std::endl;
                return;
            }
            std::string line;
            while (std::getline(cpuInfoFile, line)) {
                if (line.find("processor") != std::string::npos) {
                    backInfo.Cpu.CpuThreadNums++;
                }
                if (backInfo.Cpu.CpuCoreNums == 0 && line.find("cpu cores") != std::string::npos) {
                    const size_t pos = line.find(':');
                    if (pos != std::string::npos) {
                        backInfo.Cpu.CpuCoreNums = std::stoi(line.substr(pos + 1));
                    }
                }
            }
            firstTime = false;
        }
        const auto newCpuTime = tackSnapShot();
        const auto TotalDiff = newCpuTime.TotalTime - backInfo.lastCpuTime.TotalTime;
        const auto IdleDiff = newCpuTime.IdleTime - backInfo.lastCpuTime.IdleTime;
        if (TotalDiff != 0) {
            backInfo.Cpu.CpuUsage = 100.0 * (TotalDiff - IdleDiff) / TotalDiff;
        } else {
            backInfo.Cpu.CpuUsage = 0.0;
        }
        this->backInfo.lastCpuTime = newCpuTime;
    }
};

GeMMonitor::GeMMonitor(QWidget *parent) : QWidget(parent), ui(new Ui::GeMMonitor) {
    ui->setupUi(this);

    memInfo = new MemInfo();
    cpuInfo = new CpuInfo();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GeMMonitor::updateSystemInfo);
    timer->start(500); // 500毫秒更新一次

    updateSystemInfo();
}

GeMMonitor::~GeMMonitor() {
    delete ui;
    delete timer;
    delete memInfo;
    delete cpuInfo;
}

void GeMMonitor::updateSystemInfo() {
    memInfo->refresh();
    cpuInfo->refresh();

    auto memInfoData = memInfo->getMemInfo();
    auto cpuInfoData = cpuInfo->getCpuInfo();

    std::stringstream cpuStream;
    cpuStream << cpuInfoData.Cpu.CpuCoreNums << " Core " << cpuInfoData.Cpu.CpuThreadNums << " Thread ";
    ui->cpuinfo->setText(QString::fromStdString(cpuStream.str()));
    ui->cpuprogressbar->setValue(static_cast<int>(cpuInfoData.Cpu.CpuUsage));

    long memUsed = memInfoData.MemTotal.value - memInfoData.MemAvailable.value;
    double memUsage = (static_cast<double>(memUsed) / memInfoData.MemTotal.value) * 100.0;

    std::stringstream memStream;
    memStream << memUsed / 1024 << "MB / " << memInfoData.MemTotal.value / 1024 << "MB ";
    ui->meminfo->setText(QString::fromStdString(memStream.str()));
    ui->memprogressbar->setValue(static_cast<int>(memUsage));
}
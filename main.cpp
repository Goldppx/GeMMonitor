#include <QApplication>
#include "GeMMonitor.h"

int main(int argc,char** argv) {
    //qwq
    auto monitor = QApplication{argc, argv};
    auto window = GeMMonitor{};
    window.show();
    return QApplication::exec();
}
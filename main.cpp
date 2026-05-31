/*
 * 三相电力负荷参数计算器 -- 电气工程研究生专用
 * v1.1 迭代版本: 新增 S / 负荷率评估
 */
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("ElectricalPowerMonitor");
    app.setApplicationVersion("1.1.0");

    MainWindow w;
    w.setWindowTitle("三相电力负荷计算器 v1.1");
    w.resize(620, 520);
    w.show();

    return app.exec();
}

/*
 * 三相电力负荷参数计算器 -- 电气工程研究生专用
 * v1.0 基础版本: P / Q 计算
 */
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("ElectricalPowerMonitor");
    app.setApplicationVersion("1.0.0");

    MainWindow w;
    w.setWindowTitle("三相电力负荷计算器 v1.0");
    w.resize(600, 450);
    w.show();

    return app.exec();
}

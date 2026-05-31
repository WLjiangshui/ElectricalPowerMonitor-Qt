/*
 * 三相电力负荷综合分析工具 -- 电气工程研究生专用
 * v1.2 迭代版本: 新增三相不平衡度 + 并联电容器无功补偿容量
 * 
 * 功能:
 *   1. 三相负荷 P/Q/S 及负荷率评估 (GB/T 13462)
 *   2. 三相电流不平衡度分析 (GB/T 15543)
 *   3. 并联电容器无功补偿容量计算
 */
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("ElectricalPowerMonitor");
    app.setApplicationVersion("1.2.0");

    MainWindow w;
    w.setWindowTitle("三相电力负荷综合分析工具 v1.2");
    w.resize(640, 600);
    w.show();

    return app.exec();
}

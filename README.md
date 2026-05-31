# 三相电力负荷参数计算器 (Three-Phase Power Load Calculator)

基于 **Qt6 / C++** 的电气工程计算工具，用于三相交流电路的关键参数计算。

## 功能

- 三相有功功率 P = √3·U·I·cosφ
- 三相无功功率 Q = √3·U·I·sinφ
- 视在功率 S = √(P²+Q²)
- 功率因数角 φ 计算
- 并联补偿电容 C 计算
- 实时功率曲线图表
- 结果导出为 CSV

## 技术栈

- Qt 6.5+ (Widgets)
- CMake 3.16+
- C++17

## 构建

```bash
mkdir build && cd build
cmake ..
cmake --build .
./ElectricalPowerMonitor
```

## Git 修改迭代过程

| 版本 | 说明 |
|------|------|
| v1.0 | 基础版本：P/Q 计算 |
| v1.1 | 新增 S/φ/C 补偿电容计算 |
| v1.2 | 新增实时图表 + CSV导出 + 输入校验 |

可通过 GitHub Commits 查看完整修改历史，每次提交均附带详细的代码变更 diff。

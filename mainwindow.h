#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLineEdit;
class QLabel;
class QPushButton;
class QComboBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onCalculate();
    void onClear();
    void onModeChanged(int index);

private:
    void setupUI();

    // 模式选择
    QComboBox *m_modeCombo;

    // 输入控件
    QLineEdit *m_voltageEdit;   // 线电压 U (V)
    QLineEdit *m_currentEdit;   // 线电流 I (A)
    QLineEdit *m_pfEdit;        // 功率因数 cos(phi)
    QLineEdit *m_loadEdit;      // 额定容量 Sn (kVA)

    // 三相不平衡输入
    QLineEdit *m_IaEdit;        // A相电流
    QLineEdit *m_IbEdit;        // B相电流
    QLineEdit *m_IcEdit;        // C相电流

    // 无功补偿输入
    QLineEdit *m_targetPFEdit;  // 目标功率因数

    // 输出标签
    QLabel *m_PLabel;           // 有功功率 P
    QLabel *m_QLabel;           // 无功功率 Q
    QLabel *m_SLabel;           // 视在功率 S
    QLabel *m_loadRateLabel;    // 负荷率
    QLabel *m_statusLabel;      // 评估状态
    QLabel *m_unbalanceLabel;   // 三相不平衡度
    QLabel *m_compLabel;        // 补偿电容容量
};

#endif // MAINWINDOW_H

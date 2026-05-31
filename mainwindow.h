#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLineEdit;
class QLabel;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onCalculate();
    void onClear();

private:
    void setupUI();

    // 输入控件
    QLineEdit *m_voltageEdit;   // 线电压 U (V)
    QLineEdit *m_currentEdit;   // 线电流 I (A)
    QLineEdit *m_pfEdit;        // 功率因数 cos(phi)

    // 输出标签
    QLabel *m_PLabel;           // 有功功率 P
    QLabel *m_QLabel;           // 无功功率 Q
};

#endif // MAINWINDOW_H

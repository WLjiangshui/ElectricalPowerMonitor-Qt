#include "mainwindow.h"

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QFrame>

#include <cmath>

static constexpr double SQRT3 = 1.73205080757;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(14);
    mainLayout->setContentsMargins(20, 14, 20, 14);

    // ====== 输入区 ======
    auto *inputGroup = new QGroupBox("输入参数");
    auto *formLayout = new QFormLayout(inputGroup);

    m_voltageEdit = new QLineEdit;
    m_voltageEdit->setPlaceholderText("例如 380");
    m_currentEdit = new QLineEdit;
    m_currentEdit->setPlaceholderText("例如 50");
    m_pfEdit = new QLineEdit;
    m_pfEdit->setPlaceholderText("0 ~ 1, 例如 0.85");
    m_loadEdit = new QLineEdit;
    m_loadEdit->setPlaceholderText("变压器额定容量, 例如 50");

    formLayout->addRow("线电压 U (V): ", m_voltageEdit);
    formLayout->addRow("线电流 I (A): ", m_currentEdit);
    formLayout->addRow("功率因数 cos(phi): ", m_pfEdit);
    formLayout->addRow("额定容量 Sn (kVA): ", m_loadEdit);

    mainLayout->addWidget(inputGroup);

    // ====== 按钮区 ======
    auto *btnLayout = new QHBoxLayout;
    auto *calcBtn = new QPushButton("计算 P / Q / S");
    calcBtn->setStyleSheet("QPushButton{font-size:14px; padding:6px 20px; background:#1890ff; color:#fff; border-radius:4px;}"
                           "QPushButton:hover{background:#40a9ff;}");
    auto *clearBtn = new QPushButton("清空");
    clearBtn->setStyleSheet("QPushButton{font-size:14px; padding:6px 20px;}");
    btnLayout->addStretch();
    btnLayout->addWidget(calcBtn);
    btnLayout->addWidget(clearBtn);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    connect(calcBtn, &QPushButton::clicked, this, &MainWindow::onCalculate);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::onClear);

    // ====== 结果区 ======
    auto *resultGroup = new QGroupBox("计算结果");
    auto *resultLayout = new QVBoxLayout(resultGroup);

    m_PLabel = new QLabel("有功功率 P = ---- W");
    m_PLabel->setStyleSheet("font-size:14px; padding:2px;");
    m_QLabel = new QLabel("无功功率 Q = ---- var");
    m_QLabel->setStyleSheet("font-size:14px; padding:2px;");
    m_SLabel = new QLabel("视在功率 S = ---- VA");
    m_SLabel->setStyleSheet("font-size:14px; padding:2px;");

    // 分隔线
    auto *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);

    m_loadRateLabel = new QLabel("负荷率 = ---- %");
    m_loadRateLabel->setStyleSheet("font-size:14px; padding:2px;");
    m_statusLabel = new QLabel("评估: 等待计算");
    m_statusLabel->setStyleSheet("font-size:13px; padding:2px; color:#555;");

    resultLayout->addWidget(m_PLabel);
    resultLayout->addWidget(m_QLabel);
    resultLayout->addWidget(m_SLabel);
    resultLayout->addWidget(sep);
    resultLayout->addWidget(m_loadRateLabel);
    resultLayout->addWidget(m_statusLabel);
    mainLayout->addWidget(resultGroup);

    mainLayout->addStretch();
}

void MainWindow::onCalculate()
{
    bool okU, okI, okPF;
    double U = m_voltageEdit->text().toDouble(&okU);
    double I = m_currentEdit->text().toDouble(&okI);
    double pf = m_pfEdit->text().toDouble(&okPF);
    double Sn = m_loadEdit->text().toDouble();  // 可选参数, 无输入则为 0

    if (!okU || !okI || !okPF) {
        QMessageBox::warning(this, "输入错误", "电压/电流/功率因数必须为有效数值!");
        return;
    }
    if (pf <= 0.0 || pf > 1.0) {
        QMessageBox::warning(this, "参数错误", "功率因数应在 (0, 1] 范围内");
        return;
    }

    double sinPhi = std::sqrt(1.0 - pf * pf);
    double P = SQRT3 * U * I * pf;
    double Q = SQRT3 * U * I * sinPhi;
    double S = SQRT3 * U * I;  // 视在功率 S = sqrt(3) * U * I

    m_PLabel->setText(QString("有功功率 P = %1 W  (%2 kW)")
                          .arg(P, 0, 'f', 2)
                          .arg(P / 1000.0, 0, 'f', 3));
    m_QLabel->setText(QString("无功功率 Q = %1 var  (%2 kvar)")
                          .arg(Q, 0, 'f', 2)
                          .arg(Q / 1000.0, 0, 'f', 3));
    m_SLabel->setText(QString("视在功率 S = %1 VA  (%2 kVA)")
                          .arg(S, 0, 'f', 2)
                          .arg(S / 1000.0, 0, 'f', 3));

    // 负荷率评估 (如果提供了额定容量)
    if (Sn > 0.0) {
        double S_kVA = S / 1000.0;
        double loadRate = S_kVA / Sn * 100.0;
        m_loadRateLabel->setText(QString("负荷率 = %1%")
                                     .arg(loadRate, 0, 'f', 1));

        QString eval;
        QString color;
        if (loadRate < 30.0) {
            eval = "轻载运行 -- 变压器利用率低, 铁耗占比偏大";
            color = "#faad14";
        } else if (loadRate <= 80.0) {
            eval = "经济运行 -- 变压器处于最佳效率区间";
            color = "#52c41a";
        } else if (loadRate <= 100.0) {
            eval = "满载运行 -- 注意温升, 不宜长期满载";
            color = "#ff7a45";
        } else {
            eval = "过载运行! 建议切负荷或增容改造";
            color = "#ff4d4f";
        }
        m_statusLabel->setText(QString("评估: %1").arg(eval));
        m_statusLabel->setStyleSheet(QString("font-size:13px; padding:2px; color:%1; font-weight:bold;").arg(color));
    } else {
        m_loadRateLabel->setText("负荷率 = (未输入额定容量)");
        m_statusLabel->setText("评估: 等待计算");
        m_statusLabel->setStyleSheet("font-size:13px; padding:2px; color:#555;");
    }
}

void MainWindow::onClear()
{
    m_voltageEdit->clear();
    m_currentEdit->clear();
    m_pfEdit->clear();
    m_loadEdit->clear();
    m_PLabel->setText("有功功率 P = ---- W");
    m_QLabel->setText("无功功率 Q = ---- var");
    m_SLabel->setText("视在功率 S = ---- VA");
    m_loadRateLabel->setText("负荷率 = ---- %");
    m_statusLabel->setText("评估: 等待计算");
    m_statusLabel->setStyleSheet("font-size:13px; padding:2px; color:#555;");
}

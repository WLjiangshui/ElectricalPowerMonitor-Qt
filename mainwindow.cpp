#include "mainwindow.h"

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QFrame>

#include <algorithm>
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
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(18, 12, 18, 12);

    // ====== 模式切换 ======
    auto *modeLayout = new QHBoxLayout;
    modeLayout->addWidget(new QLabel("计算模式:"));
    m_modeCombo = new QComboBox;
    m_modeCombo->addItems({"三相负荷计算 (P/Q/S)", "三相不平衡度分析", "无功补偿容量计算"});
    modeLayout->addWidget(m_modeCombo);
    modeLayout->addStretch();
    mainLayout->addLayout(modeLayout);

    // ====== 公共输入区 ======
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

    // ====== 扩展输入区(不平衡/补偿) ======
    auto *extGroup = new QGroupBox("扩展参数");
    auto *extForm = new QFormLayout(extGroup);

    m_IaEdit = new QLineEdit;
    m_IaEdit->setPlaceholderText("A相电流 (A)");
    m_IbEdit = new QLineEdit;
    m_IbEdit->setPlaceholderText("B相电流 (A)");
    m_IcEdit = new QLineEdit;
    m_IcEdit->setPlaceholderText("C相电流 (A)");
    m_targetPFEdit = new QLineEdit;
    m_targetPFEdit->setPlaceholderText("目标功率因数, 例如 0.95");

    extForm->addRow("Ia (A): ", m_IaEdit);
    extForm->addRow("Ib (A): ", m_IbEdit);
    extForm->addRow("Ic (A): ", m_IcEdit);
    extForm->addRow("目标 cos(phi): ", m_targetPFEdit);

    mainLayout->addWidget(extGroup);

    // ====== 按钮区 ======
    auto *btnLayout = new QHBoxLayout;
    auto *calcBtn = new QPushButton("计算");
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
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onModeChanged);

    // ====== 结果区 ======
    auto *resultGroup = new QGroupBox("计算结果");
    auto *resultLayout = new QVBoxLayout(resultGroup);

    m_PLabel = new QLabel("有功功率 P = ---- W");
    m_PLabel->setStyleSheet("font-size:14px; padding:2px;");
    m_QLabel = new QLabel("无功功率 Q = ---- var");
    m_QLabel->setStyleSheet("font-size:14px; padding:2px;");
    m_SLabel = new QLabel("视在功率 S = ---- VA");
    m_SLabel->setStyleSheet("font-size:14px; padding:2px;");

    auto *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);

    m_loadRateLabel = new QLabel("负荷率 = ---- %");
    m_loadRateLabel->setStyleSheet("font-size:14px; padding:2px;");
    m_statusLabel = new QLabel("评估: 等待计算");
    m_statusLabel->setStyleSheet("font-size:13px; padding:2px; color:#555;");

    auto *sep2 = new QFrame;
    sep2->setFrameShape(QFrame::HLine);
    sep2->setFrameShadow(QFrame::Sunken);

    m_unbalanceLabel = new QLabel("三相不平衡度 = ---- %");
    m_unbalanceLabel->setStyleSheet("font-size:14px; padding:2px;");
    m_compLabel = new QLabel("补偿容量 = ---- kvar");
    m_compLabel->setStyleSheet("font-size:14px; padding:2px;");

    resultLayout->addWidget(m_PLabel);
    resultLayout->addWidget(m_QLabel);
    resultLayout->addWidget(m_SLabel);
    resultLayout->addWidget(sep);
    resultLayout->addWidget(m_loadRateLabel);
    resultLayout->addWidget(m_statusLabel);
    resultLayout->addWidget(sep2);
    resultLayout->addWidget(m_unbalanceLabel);
    resultLayout->addWidget(m_compLabel);
    mainLayout->addWidget(resultGroup);

    mainLayout->addStretch();
}

void MainWindow::onModeChanged(int /*index*/)
{
    // 保留所有字段可见, 按需计算
}

void MainWindow::onCalculate()
{
    bool okU, okI, okPF;
    double U = m_voltageEdit->text().toDouble(&okU);
    double I = m_currentEdit->text().toDouble(&okI);
    double pf = m_pfEdit->text().toDouble(&okPF);
    double Sn = m_loadEdit->text().toDouble();

    // --- 三相负荷 P/Q/S 计算 ---
    if (okU && okI && okPF) {
        if (pf <= 0.0 || pf > 1.0) {
            QMessageBox::warning(this, "参数错误", "功率因数应在 (0, 1] 范围内");
            return;
        }
        double sinPhi = std::sqrt(1.0 - pf * pf);
        double P = SQRT3 * U * I * pf;
        double Q = SQRT3 * U * I * sinPhi;
        double S = SQRT3 * U * I;

        m_PLabel->setText(QString("有功功率 P = %1 W  (%2 kW)")
                              .arg(P, 0, 'f', 2)
                              .arg(P / 1000.0, 0, 'f', 3));
        m_QLabel->setText(QString("无功功率 Q = %1 var  (%2 kvar)")
                              .arg(Q, 0, 'f', 2)
                              .arg(Q / 1000.0, 0, 'f', 3));
        m_SLabel->setText(QString("视在功率 S = %1 VA  (%2 kVA)")
                              .arg(S, 0, 'f', 2)
                              .arg(S / 1000.0, 0, 'f', 3));

        // 负荷率评估
        if (Sn > 0.0) {
            double S_kVA = S / 1000.0;
            double loadRate = S_kVA / Sn * 100.0;
            m_loadRateLabel->setText(QString("负荷率 = %1%")
                                         .arg(loadRate, 0, 'f', 1));
            QString eval, color;
            if (loadRate < 30.0) {
                eval = "轻载运行 -- 变压器利用率低, 铁耗占比偏大"; color = "#faad14";
            } else if (loadRate <= 80.0) {
                eval = "经济运行 -- 变压器处于最佳效率区间"; color = "#52c41a";
            } else if (loadRate <= 100.0) {
                eval = "满载运行 -- 注意温升, 不宜长期满载"; color = "#ff7a45";
            } else {
                eval = "过载运行! 建议切负荷或增容改造"; color = "#ff4d4f";
            }
            m_statusLabel->setText(QString("评估: %1").arg(eval));
            m_statusLabel->setStyleSheet(QString("font-size:13px; padding:2px; color:%1; font-weight:bold;").arg(color));
        } else {
            m_loadRateLabel->setText("负荷率 = (未输入额定容量)");
            m_statusLabel->setText("评估: 等待计算");
            m_statusLabel->setStyleSheet("font-size:13px; padding:2px; color:#555;");
        }

        // 补偿容量计算: Qc = P*(tan(phi1) - tan(phi2))
        double targetPF = m_targetPFEdit->text().toDouble();
        if (targetPF > 0.0 && targetPF <= 1.0 && pf > 0.0) {
            double tan1 = std::sqrt(1.0 - pf * pf) / pf;
            double tan2 = std::sqrt(1.0 - targetPF * targetPF) / targetPF;
            double Qc = P * (tan1 - tan2);
            m_compLabel->setText(QString("补偿容量 = %1 kvar  (cos(phi): %2 -> %3)")
                                     .arg(Qc / 1000.0, 0, 'f', 2)
                                     .arg(pf)
                                     .arg(targetPF));
        } else {
            m_compLabel->setText("补偿容量 = (需输入目标功率因数)");
        }
    } else if (!okU || !okI || !okPF) {
        m_PLabel->setText("有功功率 P = (需完整输入U/I/PF)");
        m_QLabel->setText("无功功率 Q =");
        m_SLabel->setText("视在功率 S =");
    }

    // --- 三相不平衡度计算 ---
    double Ia = m_IaEdit->text().toDouble();
    double Ib = m_IbEdit->text().toDouble();
    double Ic = m_IcEdit->text().toDouble();
    if (Ia > 0.0 && Ib > 0.0 && Ic > 0.0) {
        double Iavg = (Ia + Ib + Ic) / 3.0;
        double maxDev = std::max({std::abs(Ia - Iavg),
                                  std::abs(Ib - Iavg),
                                  std::abs(Ic - Iavg)});
        double unbalance = maxDev / Iavg * 100.0;
        m_unbalanceLabel->setText(QString("三相不平衡度 = %1%  (GB/T 15543 限值: 2%短时/4%长时)")
                                      .arg(unbalance, 0, 'f', 2));
        if (unbalance > 4.0) {
            m_unbalanceLabel->setStyleSheet("font-size:14px; padding:2px; color:#ff4d4f;");
        } else {
            m_unbalanceLabel->setStyleSheet("font-size:14px; padding:2px; color:#52c41a;");
        }
    } else {
        m_unbalanceLabel->setText("三相不平衡度 = (需输入三相电流)");
        m_unbalanceLabel->setStyleSheet("font-size:14px; padding:2px;");
    }
}

void MainWindow::onClear()
{
    m_voltageEdit->clear();
    m_currentEdit->clear();
    m_pfEdit->clear();
    m_loadEdit->clear();
    m_IaEdit->clear();
    m_IbEdit->clear();
    m_IcEdit->clear();
    m_targetPFEdit->clear();

    m_PLabel->setText("有功功率 P = ---- W");
    m_QLabel->setText("无功功率 Q = ---- var");
    m_SLabel->setText("视在功率 S = ---- VA");
    m_loadRateLabel->setText("负荷率 = ---- %");
    m_statusLabel->setText("评估: 等待计算");
    m_statusLabel->setStyleSheet("font-size:13px; padding:2px; color:#555;");
    m_unbalanceLabel->setText("三相不平衡度 = ---- %");
    m_unbalanceLabel->setStyleSheet("font-size:14px; padding:2px;");
    m_compLabel->setText("补偿容量 = ---- kvar");
}

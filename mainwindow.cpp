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
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(20, 16, 20, 16);

    // ====== 输入区 ======
    auto *inputGroup = new QGroupBox("输入参数");
    auto *formLayout = new QFormLayout(inputGroup);

    m_voltageEdit = new QLineEdit;
    m_voltageEdit->setPlaceholderText("例如 380");
    m_currentEdit = new QLineEdit;
    m_currentEdit->setPlaceholderText("例如 50");
    m_pfEdit = new QLineEdit;
    m_pfEdit->setPlaceholderText("0 ~ 1, 例如 0.85");

    formLayout->addRow("线电压 U (V): ", m_voltageEdit);
    formLayout->addRow("线电流 I (A): ", m_currentEdit);
    formLayout->addRow("功率因数 cos(phi): ", m_pfEdit);

    mainLayout->addWidget(inputGroup);

    // ====== 按钮 ======
    auto *btnLayout = new QHBoxLayout;
    auto *calcBtn = new QPushButton("计算 P / Q");
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
    m_PLabel->setStyleSheet("font-size:15px; padding:4px;");
    m_QLabel = new QLabel("无功功率 Q = ---- var");
    m_QLabel->setStyleSheet("font-size:15px; padding:4px;");

    resultLayout->addWidget(m_PLabel);
    resultLayout->addWidget(m_QLabel);
    mainLayout->addWidget(resultGroup);

    mainLayout->addStretch();
}

void MainWindow::onCalculate()
{
    bool okU, okI, okPF;
    double U = m_voltageEdit->text().toDouble(&okU);
    double I = m_currentEdit->text().toDouble(&okI);
    double pf = m_pfEdit->text().toDouble(&okPF);

    if (!okU || !okI || !okPF) {
        QMessageBox::warning(this, "输入错误", "请输入有效数值!");
        return;
    }
    if (pf <= 0.0 || pf > 1.0) {
        QMessageBox::warning(this, "参数错误", "功率因数应在 (0, 1] 范围内");
        return;
    }

    double sinPhi = std::sqrt(1.0 - pf * pf);
    double P = SQRT3 * U * I * pf;
    double Q = SQRT3 * U * I * sinPhi;

    m_PLabel->setText(QString("有功功率 P = %1 W  (%2 kW)")
                          .arg(P, 0, 'f', 2)
                          .arg(P / 1000.0, 0, 'f', 3));
    m_QLabel->setText(QString("无功功率 Q = %1 var  (%2 kvar)")
                          .arg(Q, 0, 'f', 2)
                          .arg(Q / 1000.0, 0, 'f', 3));
}

void MainWindow::onClear()
{
    m_voltageEdit->clear();
    m_currentEdit->clear();
    m_pfEdit->clear();
    m_PLabel->setText("有功功率 P = ---- W");
    m_QLabel->setText("无功功率 Q = ---- var");
}

#include "mainwindow.h"
#include "trafficwidget.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QRandomGenerator>

namespace {
struct Rule {
    QString chain;
    QString action;
    QString protocol;
    QString source;
    QString destination;
    QString port;
    QString interfaceName;
    QString packets;
};

const QList<Rule> demoRules = {
    {"INPUT", "ACCEPT", "tcp", "192.168.1.0/24", "0.0.0.0/0", "22", "eth0", "1 284"},
    {"INPUT", "ACCEPT", "tcp", "0.0.0.0/0", "0.0.0.0/0", "80, 443", "eth0", "15 902"},
    {"INPUT", "DROP", "all", "10.20.30.40", "0.0.0.0/0", "—", "*", "37"},
    {"FORWARD", "ACCEPT", "all", "172.18.0.0/16", "0.0.0.0/0", "—", "docker0", "8 411"},
    {"OUTPUT", "ACCEPT", "udp", "0.0.0.0/0", "8.8.8.8", "53", "*", "2 106"},
};

QColor actionColor(const QString &action)
{
    return action == "ACCEPT" ? QColor("#dcfce7") : QColor("#fee2e2");
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("iptables — правила firewall");
    resize(1120, 640);

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(24, 20, 24, 24);
    layout->setSpacing(14);

    auto *title = new QLabel("Правила iptables", central);
    title->setObjectName("title");
    auto *subtitle = new QLabel("Просмотр активных правил сетевого экрана", central);
    subtitle->setObjectName("subtitle");
    layout->addWidget(title);
    layout->addWidget(subtitle);

    auto *toolbar = new QFrame(central);
    auto *toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setContentsMargins(0, 8, 0, 8);
    toolbarLayout->addWidget(new QLabel("Цепочка:", toolbar));
    chainFilter_ = new QComboBox(toolbar);
    chainFilter_->addItems({"Все", "INPUT", "FORWARD", "OUTPUT"});
    toolbarLayout->addWidget(chainFilter_);
    toolbarLayout->addStretch();
    ruleCount_ = new QLabel(toolbar);
    toolbarLayout->addWidget(ruleCount_);
    auto *refreshButton = new QPushButton("Обновить", toolbar);
    toolbarLayout->addWidget(refreshButton);
    layout->addWidget(toolbar);

    rulesTable_ = new QTableWidget(0, 8, central);
    rulesTable_->setHorizontalHeaderLabels({
        "Цепочка", "Действие", "Протокол", "Источник",
        "Назначение", "Порт", "Интерфейс", "Пакеты"
    });
    rulesTable_->setAlternatingRowColors(true);
    rulesTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    rulesTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    rulesTable_->verticalHeader()->setVisible(false);
    rulesTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(rulesTable_);

    auto *trafficTitle = new QLabel("Трафик сети в реальном времени", central);
    trafficTitle->setObjectName("sectionTitle");
    auto *trafficHeader = new QHBoxLayout;
    trafficHeader->addWidget(trafficTitle);
    trafficHeader->addStretch();
    trafficStats_ = new QLabel("● Входящий 0.0 Mbps    ● Исходящий 0.0 Mbps", central);
    trafficStats_->setObjectName("trafficStats");
    trafficHeader->addWidget(trafficStats_);
    layout->addLayout(trafficHeader);
    trafficChart_ = new TrafficWidget(central);
    layout->addWidget(trafficChart_);

    setCentralWidget(central);
    setStyleSheet(R"(
        QMainWindow, QWidget { background: #f8fafc; color: #172033; font-size: 14px; }
        QLabel#title { font-size: 28px; font-weight: 700; }
        QLabel#subtitle { color: #64748b; margin-bottom: 8px; }
        QLabel#sectionTitle { font-size: 18px; font-weight: 700; margin-top: 8px; }
        QLabel#trafficStats { color: #475569; font-weight: 600; }
        QTableWidget { background: white; border: 1px solid #dbe3ee; border-radius: 8px; gridline-color: #e8edf4; }
        QHeaderView::section { background: #eef2f7; border: 0; border-bottom: 1px solid #dbe3ee; padding: 10px 6px; font-weight: 600; }
        QTableWidget::item { padding: 8px; }
        QComboBox, QPushButton { background: white; border: 1px solid #cbd5e1; border-radius: 6px; padding: 7px 12px; }
        QPushButton:hover { background: #eef2ff; border-color: #818cf8; }
    )");

    populateRules();
    connect(chainFilter_, &QComboBox::currentTextChanged, this, &MainWindow::applyFilter);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::populateRules);

    auto *trafficTimer = new QTimer(this);
    connect(trafficTimer, &QTimer::timeout, this, &MainWindow::updateTraffic);
    trafficTimer->start(1000);
    for (int i = 0; i < 20; ++i)
        updateTraffic();
}

void MainWindow::populateRules()
{
    rulesTable_->setRowCount(0);
    for (const auto &rule : demoRules) {
        const int row = rulesTable_->rowCount();
        rulesTable_->insertRow(row);
        const QStringList values = {rule.chain, rule.action, rule.protocol, rule.source,
                                    rule.destination, rule.port, rule.interfaceName, rule.packets};
        for (int column = 0; column < values.size(); ++column) {
            auto *item = new QTableWidgetItem(values.at(column));
            if (column == 1)
                item->setBackground(actionColor(rule.action));
            rulesTable_->setItem(row, column, item);
        }
    }
    applyFilter(chainFilter_->currentText());
}

void MainWindow::applyFilter(const QString &chain)
{
    int visible = 0;
    for (int row = 0; row < rulesTable_->rowCount(); ++row) {
        const bool show = chain == "Все" || rulesTable_->item(row, 0)->text() == chain;
        rulesTable_->setRowHidden(row, !show);
        visible += show ? 1 : 0;
    }
    ruleCount_->setText(QString("Правил: %1").arg(visible));
}

void MainWindow::updateTraffic()
{
    auto *random = QRandomGenerator::global();
    incomingMbps_ = qBound(1.0, incomingMbps_ + random->bounded(8.0) - 4.0, 95.0);
    outgoingMbps_ = qBound(0.5, outgoingMbps_ + random->bounded(4.0) - 2.0, 55.0);
    if (random->bounded(12) == 0)
        incomingMbps_ = qMin(95.0, incomingMbps_ + random->bounded(18.0));
    trafficChart_->addSample(incomingMbps_, outgoingMbps_);
    trafficStats_->setText(QString("<span style='color:#2563eb'>●</span> Входящий %1 Mbps &nbsp;&nbsp; "
                                   "<span style='color:#f97316'>●</span> Исходящий %2 Mbps")
                               .arg(incomingMbps_, 0, 'f', 1)
                               .arg(outgoingMbps_, 0, 'f', 1));
}


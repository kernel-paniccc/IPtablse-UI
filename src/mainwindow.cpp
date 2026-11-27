#include "mainwindow.h"
#include "iptablesloader.h"
#include "trafficwidget.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QSpinBox>

namespace {
QColor actionColor(const QString &action)
{
    return action == "ACCEPT" ? QColor("#dcfce7") : QColor("#fee2e2");
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("iptables — правила firewall");
    resize(1120, 760);

    auto *tabs = new QTabWidget(this);
    auto *central = new QWidget(tabs);
    auto *profilePage = new QWidget(tabs);
    tabs->addTab(central, "Мониторинг");
    tabs->addTab(profilePage, "Конструктор");
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(24, 20, 24, 24);
    layout->setSpacing(14);

    auto *title = new QLabel("Правила iptables", central);
    title->setObjectName("title");
    sourceStatus_ = new QLabel("Загрузка активных правил сетевого экрана…", central);
    sourceStatus_->setObjectName("subtitle");
    layout->addWidget(title);
    layout->addWidget(sourceStatus_);

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
    rulesTable_->setMinimumHeight(250);
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

    auto *profileLayout = new QVBoxLayout(profilePage);
    auto *profileTable = new QTableWidget(0, 4, profilePage);
    profileTable->setHorizontalHeaderLabels({"Цепочка", "Протокол", "Порт назначения", "Действие"});
    profileTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auto *profileButtons = new QHBoxLayout;
    auto *editRuleButton = new QPushButton("Изменить", profilePage);
    auto *deleteRuleButton = new QPushButton("Удалить", profilePage);

    auto *addRuleButton = new QPushButton("Добавить", profilePage);
    profileButtons->addWidget(addRuleButton);
    profileButtons->addWidget(editRuleButton);
    profileButtons->addWidget(deleteRuleButton);
    profileButtons->addStretch();

    profileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    profileTable->setSelectionMode(QAbstractItemView::SingleSelection);

    editRuleButton->setEnabled(false);
    deleteRuleButton->setEnabled(false);

    connect(profileTable, &QTableWidget::itemSelectionChanged, this,
            [profileTable, editRuleButton, deleteRuleButton] {
        const bool hasSelection = !profileTable->selectedItems().isEmpty();
        editRuleButton->setEnabled(hasSelection);
        deleteRuleButton->setEnabled(hasSelection);
    });
    auto *chainLayout = new QHBoxLayout;
    chainLayout->addWidget(new QLabel("Цепочка:", profilePage));

    auto *chainSelector = new QComboBox(profilePage);
    chainSelector->addItems({"INPUT", "OUTPUT", "FORWARD"});
    chainLayout->addWidget(chainSelector);
    chainLayout->addStretch();

    profileLayout->addLayout(chainLayout);
    auto *protocolLayout = new QHBoxLayout;
    protocolLayout->addWidget(new QLabel("Протокол:", profilePage));

    auto *protocolSelector = new QComboBox(profilePage);
    protocolSelector->addItems({"TCP", "UDP"});
    protocolLayout->addWidget(protocolSelector);
    protocolLayout->addStretch();

    profileLayout->addLayout(protocolLayout);
    auto *portLayout = new QHBoxLayout;
    portLayout->addWidget(new QLabel("Порт назначения:", profilePage));

    auto *portInput = new QSpinBox(profilePage);
    portInput->setRange(1, 65535);
    portInput->setValue(443);
    portLayout->addWidget(portInput);
    portLayout->addStretch();

    profileLayout->addLayout(portLayout);
    auto *actionLayout = new QHBoxLayout;
    actionLayout->addWidget(new QLabel("Действие:", profilePage));

    auto *actionSelector = new QComboBox(profilePage);
    actionSelector->addItems({"ACCEPT", "DROP"});
    actionLayout->addWidget(actionSelector);
    actionLayout->addStretch();

    profileLayout->addLayout(actionLayout);
    profileLayout->addLayout(profileButtons);
    auto *previewButton = new QPushButton("Предпросмотр", profilePage);
    profileLayout->addWidget(previewButton);

    auto *profilePreview = new QLabel("Выбери параметры и нажми «Предпросмотр».", profilePage);
    profilePreview->setWordWrap(true);
    profileLayout->addWidget(profilePreview);

    connect(previewButton, &QPushButton::clicked, this,
            [chainSelector, protocolSelector, portInput, actionSelector, profilePreview] {
        profilePreview->setText(
            QString("Цепочка: %1 | Протокол: %2 | Порт: %3 | Действие: %4")
                .arg(chainSelector->currentText())
                .arg(protocolSelector->currentText())
                .arg(portInput->value())
                .arg(actionSelector->currentText()));
    });
    const auto clearPreview = [profilePreview] {
        profilePreview->setText("Выбери параметры и нажми «Предпросмотр».");
    };

    connect(chainSelector, &QComboBox::currentIndexChanged, this, clearPreview);
    connect(protocolSelector, &QComboBox::currentIndexChanged, this, clearPreview);
    connect(portInput, &QSpinBox::valueChanged, this, clearPreview);
    connect(actionSelector, &QComboBox::currentIndexChanged, this, clearPreview);
    auto *resetButton = new QPushButton("Сбросить форму", profilePage);
    profileLayout->addWidget(resetButton);

    connect(resetButton, &QPushButton::clicked, this,
            [chainSelector, protocolSelector, portInput, actionSelector, clearPreview] {
        chainSelector->setCurrentIndex(0);
        protocolSelector->setCurrentIndex(0);
        portInput->setValue(443);
        actionSelector->setCurrentIndex(0);
        clearPreview();
    });
    connect(addRuleButton, &QPushButton::clicked, this,
            [profileTable, chainSelector, protocolSelector, portInput, actionSelector]() {
        const int row = profileTable->rowCount();
        profileTable->insertRow(row);
        profileTable->setItem(row, 0, new QTableWidgetItem(chainSelector->currentText()));
        profileTable->setItem(row, 1, new QTableWidgetItem(protocolSelector->currentText()));
        profileTable->setItem(row, 2, new QTableWidgetItem(QString::number(portInput->value())));
        profileTable->setItem(row, 3, new QTableWidgetItem(actionSelector->currentText()));
    });
    connect(deleteRuleButton, &QPushButton::clicked, this, [profileTable]() {
        const int row = profileTable->currentRow();
        if (row >= 0) {
            profileTable->removeRow(row);
        }
    });
    connect(profileTable, &QTableWidget::cellDoubleClicked, this,
            [profileTable, chainSelector, protocolSelector, portInput, actionSelector](int row, int) {
        chainSelector->setCurrentText(profileTable->item(row, 0)->text());
        protocolSelector->setCurrentText(profileTable->item(row, 1)->text());
        portInput->setValue(profileTable->item(row, 2)->text().toInt());
        actionSelector->setCurrentText(profileTable->item(row, 3)->text());
    });

    connect(editRuleButton, &QPushButton::clicked, this,
            [profileTable, chainSelector, protocolSelector, portInput, actionSelector]() {
        const int row = profileTable->currentRow();
        if (row < 0) {
            return;
        }

        profileTable->item(row, 0)->setText(chainSelector->currentText());
        profileTable->item(row, 1)->setText(protocolSelector->currentText());
        profileTable->item(row, 2)->setText(QString::number(portInput->value()));
        profileTable->item(row, 3)->setText(actionSelector->currentText());
    });
    profileLayout->addWidget(profileTable);

    setCentralWidget(tabs);
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
    networkMonitor_.sampleMbps();
    trafficTimer->start(1000);
}

void MainWindow::populateRules()
{
    rulesTable_->setRowCount(0);
    QString error;
    const auto rules = IptablesLoader().load(&error);
    sourceStatus_->setText(error.isEmpty()
        ? QString("Источник: iptables-save · загружено правил: %1").arg(rules.size())
        : error);
    sourceStatus_->setStyleSheet(error.isEmpty() ? QString() : "color: #b45309;");
    for (const auto &rule : rules) {
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
    const auto traffic = networkMonitor_.sampleMbps();
    trafficChart_->addSample(traffic.first, traffic.second);
    trafficStats_->setText(QString("<span style='color:#2563eb'>●</span> Входящий %1 Mbps &nbsp;&nbsp; "
                                   "<span style='color:#f97316'>●</span> Исходящий %2 Mbps")
                               .arg(traffic.first, 0, 'f', 2)
                               .arg(traffic.second, 0, 'f', 2));
}

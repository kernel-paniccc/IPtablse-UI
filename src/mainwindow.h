#pragma once

#include "networkmonitor.h"

#include <QMainWindow>

class QComboBox;
class QLabel;
class QTableWidget;
class TrafficWidget;

class MainWindow final : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void populateRules();
    void applyFilter(const QString &chain);
    void updateTraffic();

    QTableWidget *rulesTable_ = nullptr;
    QComboBox *chainFilter_ = nullptr;
    QLabel *ruleCount_ = nullptr;
    QLabel *sourceStatus_ = nullptr;
    QLabel *trafficStats_ = nullptr;
    TrafficWidget *trafficChart_ = nullptr;
    NetworkMonitor networkMonitor_;
};


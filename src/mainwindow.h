#pragma once

#include <QMainWindow>

class QComboBox;
class QLabel;
class QTableWidget;

class MainWindow final : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void populateRules();
    void applyFilter(const QString &chain);

    QTableWidget *rulesTable_ = nullptr;
    QComboBox *chainFilter_ = nullptr;
    QLabel *ruleCount_ = nullptr;
};


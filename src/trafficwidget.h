#pragma once

#include <QList>
#include <QWidget>

class TrafficWidget final : public QWidget
{
public:
    explicit TrafficWidget(QWidget *parent = nullptr);

    void addSample(double incomingMbps, double outgoingMbps);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    static constexpr int MaxSamples = 60;
    QList<double> incoming_;
    QList<double> outgoing_;
};


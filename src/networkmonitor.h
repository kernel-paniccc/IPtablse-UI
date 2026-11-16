#pragma once

#include <QElapsedTimer>
#include <QPair>

class NetworkMonitor
{
public:
    QPair<double, double> sampleMbps();

private:
    QPair<quint64, quint64> readByteCounters() const;

    QElapsedTimer elapsed_;
    quint64 previousRx_ = 0;
    quint64 previousTx_ = 0;
    bool initialized_ = false;
};


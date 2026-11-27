#pragma once

#include "trafficconnector.h"

#include <QElapsedTimer>
#include <QPair>

class NetworkMonitor final : public TrafficConnector
{
public:
    QPair<double, double> sampleMbps() override;
    bool isAvailable() const override;

    bool lastReadFailed() const { return lastReadFailed_; }

private:
    QPair<quint64, quint64> readByteCounters();

    QElapsedTimer elapsed_;
    quint64 previousRx_ = 0;
    quint64 previousTx_ = 0;
    bool initialized_ = false;
    bool lastReadFailed_ = false;
};
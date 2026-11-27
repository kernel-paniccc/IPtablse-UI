#pragma once
#include <QDateTime>
struct TrafficSample {
    QDateTime timestamp;
    double incomingMbps = 0.0;
    double outgoingMbps = 0.0;
};
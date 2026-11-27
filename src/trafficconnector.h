#pragma once
#include <QPair>
class TrafficConnector {
public:
    virtual ~TrafficConnector() = default;
    virtual QPair<double,double> sampleMbps() = 0;
    virtual bool isAvailable() const = 0;
};
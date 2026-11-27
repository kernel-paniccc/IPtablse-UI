#pragma once
#include "iptablesloader.h"   // используем существующий IptablesRule
#include <QList>
class IptablesConnector {
public:
    virtual ~IptablesConnector() = default;
    virtual QList<IptablesRule> loadRules(QString* errorMessage = nullptr) const = 0;
};
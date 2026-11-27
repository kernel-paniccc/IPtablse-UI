#pragma once

#include "iptablesrule.h"

#include <QList>
#include <QString>

class IptablesConnector
{
public:
    virtual ~IptablesConnector() = default;
    virtual QList<IptablesRule> loadRules(QString* errorMessage = nullptr) const = 0;
};
#pragma once

#include "iptablesconnector.h"

#include <QList>
#include <QString>

class IptablesLoader final : public IptablesConnector
{
public:
    QList<IptablesRule> loadRules(QString* errorMessage = nullptr) const override;

    QList<IptablesRule> load(QString* errorMessage = nullptr) const
    {
        return loadRules(errorMessage);
    }
};
#pragma once

#include <QList>
#include <QString>
#include "iptablesconnector.h"
class IptablesLoader final : public IptablesConnector {
public:
    QList<IptablesRule> loadRules(QString* errorMessage = nullptr) const override;
    QList<IptablesRule> load(QString* e = nullptr) const { return loadRules(e); }
};

struct IptablesRule
{
    QString chain;
    QString action;
    QString protocol;
    QString source;
    QString destination;
    QString port;
    QString interfaceName;
    QString packets;
};

class IptablesLoader
{
public:
    QList<IptablesRule> load(QString *errorMessage = nullptr) const;
};


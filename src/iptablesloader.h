#pragma once

#include <QList>
#include <QString>

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


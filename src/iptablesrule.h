#pragma once

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
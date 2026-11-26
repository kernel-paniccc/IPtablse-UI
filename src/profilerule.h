#pragma once
#include <QString>

struct ProfileRule {
    int id;
    QString chain;
    QString protocol;
    int destinationPort;
    QString action;

    bool operator==(const ProfileRule& other) const {
        return id == other.id && chain == other.chain && protocol == other.protocol
            && destinationPort == other.destinationPort && action == other.action;
    }
};
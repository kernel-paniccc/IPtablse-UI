#pragma once

#include "profilerule.h"
#include <vector>
#include <QString>
#include <optional>

class RuleProfile {
public:
    RuleProfile() = default;

    bool addRule(ProfileRule& rule, QString* errorMessage = nullptr);
    bool updateRule(const ProfileRule& rule, QString* errorMessage = nullptr);
    bool removeRule(int id, QString* errorMessage = nullptr);

    const std::vector<ProfileRule>& rules() const { return rules_; }
    std::optional<ProfileRule> getRuleById(int id) const;
    std::vector<ProfileRule> filterByProtocol(const QString& proto) const;
    std::vector<ProfileRule> filterByPort(int port) const;
    
    void clear() { rules_.clear(); nextId_ = 1; }

    static bool validateRule(const ProfileRule& rule, QString* errorMessage = nullptr);

private:
    std::vector<ProfileRule> rules_;
    int nextId_ = 1;
};
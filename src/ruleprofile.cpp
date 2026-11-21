#include "ruleprofile.h"
#include <algorithm>

// Добавление правила с уникальным идентификатором
bool RuleProfile::addRule(ProfileRule& rule, QString* errorMessage)
{
    if (!validateRule(rule, errorMessage)) {
        return false;
    }

    // Присваиваем уникальный ID и увеличиваем счетчик
    rule.id = nextId_++;
    rules_.push_back(rule);
    return true;
}

// Вспомогательный метод для получения правила по ID
std::optional<ProfileRule> RuleProfile::getRuleById(int id) const
{
    for (const auto& rule : rules_) {
        if (rule.id == id) {
            return rule;
        }
    }
    return std::nullopt;
}
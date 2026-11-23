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

bool RuleProfile::updateRule(const ProfileRule& rule, QString* errorMessage)
{
    if (!validateRule(rule, errorMessage)) return false;
    for (auto& r : rules_) {
        if (r.id == rule.id) { r = rule; return true; }
    }
    if (errorMessage) *errorMessage = "Правило с ID " + QString::number(rule.id) + " не найдено.";
    return false;
}

bool RuleProfile::removeRule(int id, QString* errorMessage)
{
    const auto it = std::find_if(rules_.begin(), rules_.end(),
        [id](const ProfileRule& r) { return r.id == id; });
    if (it == rules_.end()) {
        if (errorMessage) *errorMessage = "Правило с ID " + QString::number(id) + " не найдено.";
        return false;
    }
    rules_.erase(it);
    return true;
}


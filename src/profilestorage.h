#pragma once

#include "profilerule.h"
#include <QList>
#include <QString>

struct StorageResult {
    bool success = false;
    QString errorMessage;
    QList<ProfileRule> rules;
};

class ProfileStorage {
public:
    virtual ~ProfileStorage() = default;

    virtual bool save(const QList<ProfileRule>& rules, const QString& filePath, QString* errorMessage = nullptr) = 0;
    
    virtual StorageResult load(const QString& filePath) = 0;
};
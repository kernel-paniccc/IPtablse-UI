#pragma once

#include "profilestorage.h"

class ProfileStorageJson final : public ProfileStorage
{
public:
    bool save(const QList<ProfileRule>& rules,
              const QString& filePath,
              QString* errorMessage = nullptr) override;

    StorageResult load(const QString& filePath) override;
};
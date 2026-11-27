#include "profilestorage_json.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

bool ProfileStorageJson::save(const QList<ProfileRule>& rules,
                              const QString& filePath,
                              QString* errorMessage)
{
    // 1. Сериализация: QList<ProfileRule> -> QJsonArray
    QJsonArray array;
    array.reserve(rules.size());
    for (const auto& rule : rules) {
        QJsonObject object;
        object["id"]              = rule.id;
        object["chain"]           = rule.chain;
        object["protocol"]        = rule.protocol;
        object["destinationPort"] = rule.destinationPort;
        object["action"]          = rule.action;
        array.append(object);
    }

    const QJsonDocument document(array);
    const QByteArray payload = document.toJson(QJsonDocument::Indented);

    // 2. Атомарная запись через QSaveFile:
    //    сначала пишем во временный файл, commit() делает rename.
    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        if (errorMessage)
            *errorMessage = QString("Не удалось открыть файл для записи: %1")
                                .arg(file.errorString());
        return false;
    }

    if (file.write(payload) != payload.size()) {
        if (errorMessage)
            *errorMessage = QString("Ошибка записи в файл: %1")
                                .arg(file.errorString());
        file.cancelWriting();
        return false;
    }

    if (!file.commit()) {
        if (errorMessage)
            *errorMessage = QString("Не удалось сохранить файл: %1")
                                .arg(file.errorString());
        return false;
    }

    return true;
}
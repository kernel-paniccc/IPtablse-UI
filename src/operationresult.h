#pragma once
#include <QString>

struct OperationResult {
    enum class Kind { None, Validation, IO, Permission, Conflict, Unavailable };
    bool success = false;
    Kind kind = Kind::None;
    QString message;

    static OperationResult ok() { return {true, Kind::None, {}}; }
    static OperationResult fail(Kind k, const QString& m) { return {false, k, m}; }
};
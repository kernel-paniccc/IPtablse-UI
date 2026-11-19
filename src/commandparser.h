#pragma once

#include <QString>
#include <QStringList>

struct ParsedCommand {
    bool isValid = false;
    QString errorMessage;
    QString command;        // filter или find
    QString argument;       // tcp, udp или номер порта
};

class CommandParser {
public:
    static ParsedCommand parse(const QString& input);
};
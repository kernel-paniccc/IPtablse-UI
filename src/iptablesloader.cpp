#include "iptablesloader.h"

#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>

QList<IptablesRule> IptablesLoader::loadRules(QString *errorMessage) const
{
    const QString executable = QStandardPaths::findExecutable("iptables-save");
    if (executable.isEmpty()) {
        if (errorMessage)
            *errorMessage = "iptables-save не найден (доступен только на Linux с iptables)";
        return {};
    }

    QProcess process;
    process.start(executable, {"-c"});
    if (!process.waitForFinished(3000)) {
        process.kill();
        if (errorMessage)
            *errorMessage = "Не удалось получить правила: превышено время ожидания";
        return {};
    }
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        if (errorMessage) {
            const QString details = QString::fromUtf8(process.readAllStandardError()).trimmed();
            *errorMessage = details.isEmpty() ? "Нет доступа к правилам iptables" : details;
        }
        return {};
    }

    QList<IptablesRule> rules;
    const auto valueAfter = [](const QStringList &tokens, const QString &option, const QString &fallback) {
        const int index = tokens.indexOf(option);
        return index >= 0 && index + 1 < tokens.size() ? tokens.at(index + 1) : fallback;
    };
    const QRegularExpression counterPattern(R"(^\[(\d+):\d+\]\s+)");
    for (QString line : QString::fromUtf8(process.readAllStandardOutput()).split('\n')) {
        const auto match = counterPattern.match(line);
        if (!match.hasMatch())
            continue;
        const QString packets = match.captured(1);
        line.remove(0, match.capturedLength());
        const QStringList tokens = line.split(' ', Qt::SkipEmptyParts);
        if (!tokens.contains("-A"))
            continue;
        rules.append({
            valueAfter(tokens, "-A", "—"),
            valueAfter(tokens, "-j", "—"),
            valueAfter(tokens, "-p", "all"),
            valueAfter(tokens, "-s", "0.0.0.0/0"),
            valueAfter(tokens, "-d", "0.0.0.0/0"),
            valueAfter(tokens, "--dport", "—"),
            valueAfter(tokens, "-i", "*"),
            packets
        });
    }
    return rules;
}


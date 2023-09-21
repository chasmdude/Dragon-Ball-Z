#include "command.h"

Command::Command(QString name, QString description, int minArgc, int maxArgc,
                 std::function<bool(QStringList &)> action)
        : name(std::move(name)),
          description(std::move(description)),
          minArgc(minArgc),
          max_argc(maxArgc),
          callback_function(std::move(action)) {
}

QString Command::toString() const {
    return QString("%1 : %2").arg(name, description);
}

const QString &Command::getCommand() const {
    return name;
}

bool Command::invoke(const QString &message) const {
    QStringList command_args = message.split(' ', Qt::SkipEmptyParts);

    if (command_args.isEmpty() || command_args[0].compare(name, Qt::CaseInsensitive) != 0) {
        return false;
    }
    command_args.removeFirst();
    qsizetype arg_count = command_args.size();
    return (minArgc <= arg_count && arg_count <= max_argc) && callback_function(command_args);
}

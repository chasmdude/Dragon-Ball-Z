#ifndef COMMAND_H
#define COMMAND_H

#include <array>
#include <functional>
#include <tuple>
#include <any>
#include <QString>
#include <QStringList>
#include <QTextStream>


using namespace std;

class Command {
public:
    Command(QString name, QString description, int minArgc, int maxArgc,
            std::function<bool(QStringList &)> action);

    [[nodiscard]] const QString &getCommand() const;

    [[nodiscard]] bool invoke(const QString &message) const;

    [[nodiscard]] QString toString() const;

private:
    const QString name;
    const QString description;
    long minArgc;
    long max_argc;
    function<bool(QStringList &)> callback_function;
};

#endif

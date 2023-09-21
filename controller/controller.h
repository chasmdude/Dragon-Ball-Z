#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "command.h"
#include <map>
#include "model/gameWorld.h"
#include "pathfinder.h"
#include <QObject>


class Controller : public QObject {
Q_OBJECT


public:
    explicit Controller(GameWorld *engine);

    ~Controller() override;

    std::vector<std::unique_ptr<Tile>> findPath(int x, int y);

public slots:

    void loadWorld(QString imgPath, int noOfEnemies, int noOfHealthPacks);

    void switchManualMode(bool manualMode);

    void changeWeight(float weight);

    void handleAutoMode();

    void commandInvoke(QString command);

    void suggest_commands(const QString &partial_command);

signals:

    void clearHistory();

    void closeTerminal();

    void setCommand(QString string);

    void appendto_Command_History(bool errLevel, QString string);
private:
    bool isManualMode = true;
    GameWorld *gameWorld;
    std::map<QString, std::unique_ptr<Command>> commands;
    std::vector<std::unique_ptr<PathFinder>> pathFinders;

    void addCommand(const QString &name, const QString &description,
                    int minA, int maxA,
                    const std::function<bool(QStringList &)> &function);

    void addDirectionCommand(const QString &name, const QString &description, GameWorld::Direction direction);

    bool handleDirection(const QStringList &params, GameWorld::Direction direction);

    bool handleGoto(const QStringList &args);

    bool handleAttack(const QStringList &args);

    bool handleHealth(const QStringList &args);

    void addBasicDirectionCommands();

    bool buildMan(const QStringList &commandList);

    float costOPath(const std::vector<std::unique_ptr<Tile>> &path);

    bool setPathIfReachable(int x, int y, float energy);

    void attackNearestEnemy(bool costnIndex1);

    bool findHealth(const QString &metric);
};

#endif

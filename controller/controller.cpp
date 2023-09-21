#include "astar.h"
#include "controller.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <ctime>
#include <stdexcept>

#include <queue>


void Controller::commandInvoke(QString command) {
    try {
        const auto &cmd = commands.at(command.split(u' ').at(0).toLower());
        if (!cmd->invoke(command)) {
            throw std::invalid_argument("Invalid Command");
        }
    }
    catch (...) {
        try {
            throw; // Re-throw the caught exception to get its details
        } catch (const std::exception &e) {
            std::cerr << "Caught exception: " << e.what() << std::endl;
        }
        emit appendto_Command_History(false, "Invalid Command");
        buildMan(QStringList());
    }
}

void Controller::suggest_commands(const QString &partial_command) {
    QStringList matchingCommands;

    for (const auto &commandPair: commands) {
        const QString &fullCommand = commandPair.second->getCommand();
        if (fullCommand.startsWith(partial_command, Qt::CaseInsensitive)) {
            matchingCommands.append(fullCommand);
        }
    }

    if (!matchingCommands.isEmpty()) {
        buildMan(matchingCommands);
        emit setCommand(matchingCommands[0]);
    } else {
        emit appendto_Command_History(true, "Unknown name. Enter 'help' for the full list of commands.");
    }
}

bool Controller::buildMan(const QStringList &commandList) {
    QStringList cmd_manual;
    if (commandList.isEmpty()) {
        for (const auto &command: commands) {
            cmd_manual << command.second->toString();
        }
    }
    for (const QString &command: commandList) {
        if (commands.find(command.toLower()) != commands.end()) {
            cmd_manual << commands[command.toLower()]->toString();
        } else {
            return false;
        }
    }
    emit appendto_Command_History(false, cmd_manual.join("\n"));
    return true;
}

float Controller::costOPath(const std::vector<std::unique_ptr<Tile>> &path) {
    return std::accumulate(path.begin(), path.end(), 0.0f,
                           [](float sum, const auto &tile) { return sum + tile->getValue(); });
}


void Controller::addCommand(const QString &name, const QString &description,
                            int mina, int maxA,
                            const std::function<bool(QStringList &)> &function) {
    commands.emplace(name, std::make_unique<Command>(name, description, mina, maxA, function));
}

bool Controller::handleDirection(const QStringList &params, GameWorld::Direction direction) {
    if (params.isEmpty()) {
        gameWorld->setProtagonistDirection(direction);
    } else {
        int n = params.at(0).toInt();
        if (n >= 0) {
            gameWorld->setProtagonistDirection(direction, n);
            return true;
        } else {
            return false;
        }
    }
    return true;
}

void Controller::addDirectionCommand(const QString &name, const QString &description, GameWorld::Direction direction) {
    addCommand(name, description, 0, 1, [=](QStringList &params) { return handleDirection(params, direction); });
}

void Controller::addBasicDirectionCommands() {
    addDirectionCommand("up", "move upwards. Parameters:\n\tn Steps (Optional)", GameWorld::Direction::Up);
    addDirectionCommand("right", "Move Right. Parameters:\n\tn Steps (Optional)",
                        GameWorld::Direction::Right);
    addDirectionCommand("down", "Move Downwards. Parameters:\n\tn Steps (Optional)",
                        GameWorld::Direction::Down);
    addDirectionCommand("left", "move to the left. Parameters:\n\tn number of steps (Optional)",
                        GameWorld::Direction::Left);
}

bool Controller::handleGoto(const QStringList &args) {
    int x = args[0].toInt();
    int y = args[1].toInt();

    if (gameWorld->isValidCoordinate(x, y)) {
        gameWorld->setPath(findPath(x, y));
        return true;
    }
    return false;
}


bool isFlagSet(const QString &flag, const QString &flag1, const QString &flag2) {
    return flag == flag1 || flag == flag2;
}

bool Controller::handleAttack(const QStringList &args) {
    if (args.isEmpty()) {
        attackNearestEnemy(false);
    } else {
        if (isFlagSet(args.at(0), "--safe", "-s")) {
            attackNearestEnemy(true);
        } else {
            return false;
        }
    }
    return true;
}

bool Controller::handleHealth(const QStringList &args) {
    if (args.isEmpty()) {
        findHealth("-n");
        return true;
    } else {
        const QString &flag = args.at(0);
        return findHealth(flag);
    }
}

Controller::Controller(GameWorld *engine) : gameWorld{engine} {
    addBasicDirectionCommands();
    addCommand("attack", "Attack the nearest enemy. Flags:\n\t--safe/-s to attack ensuring Survival after attack",
               0, 1, [=](const QStringList &params) { return handleAttack(params); });

    addCommand("clear", "clears the terminal screen", 0, 0, [=](const QStringList &) {
        emit clearHistory();
        return true;
    });
    addCommand("exit", "closes the terminal", 0, 0, [=](const QStringList &) {
        emit closeTerminal();
        return true;
    });
    addCommand("goto",
               "move to coordinates (x, y) (use zero based indexing and x,y increase rightwards and downwards respectively. Parameters:\n\tx y",
               2, 2, [=](QStringList params) { return handleGoto(params); });

    addCommand("heal",
               "Restore Health. Flags:\n\t--nearest/-n to find the closest health (default)\n\t--best/-b to find the optimal health",
               0, 1, [=](const QStringList &params) { return handleHealth(params); });
    addCommand("man", "Prints Manual", 0, 1, [=](const QStringList &cmdList) {
        return buildMan(cmdList);
    });
}

Controller::~Controller() {
    pathFinders.clear();
    commands.clear();
}

void Controller::loadWorld(QString imgPath, int noOfEnemies, int noOfHealthPacks) {
    gameWorld->loadWorld(imgPath, noOfEnemies, noOfHealthPacks);
    pathFinders.clear();
    pathFinders.reserve(1);
    auto aStar = std::make_unique<AStar>(gameWorld);
    connect(gameWorld, &GameWorld::becomeWall, aStar.get(), &AStar::makeUnreachable);
    pathFinders.emplace_back(std::move(aStar));
    handleAutoMode();
}

void Controller::switchManualMode(bool manualMode) {
    this->isManualMode = manualMode;
    handleAutoMode();
}

void Controller::changeWeight(float weight) {
    for (auto &pathfinder: pathFinders)
        pathfinder->setWeight(weight / 100.0f);
}

std::vector<std::unique_ptr<Tile>> Controller::findPath(int x, int y) {
    if (!gameWorld->isReachable(x, y)) {
        emit appendto_Command_History(true, "Invalid location to move to.");
        return {};
    }
    std::vector<std::unique_ptr<Tile>> path = pathFinders.at(0)->findPath(QPoint(x, y));
    return path;
}

bool Controller::setPathIfReachable(int x, int y, float energy) {
    auto path = findPath(x, y);
    if (!path.empty() && costOPath(path) <= energy) {
        gameWorld->setPath(std::move(path));
        return true;
    }
    return false;
}

void Controller::attackNearestEnemy(bool shouldSurvive) {
    const auto &enemies = gameWorld->getEnemies();
    if (enemies.empty()) {
        emit appendto_Command_History(true, "No enemies to attack!");
        return;
    }

    std::priority_queue<std::pair<float, unsigned int>,
            std::vector<std::pair<float, unsigned int>>,
            std::function<bool(const std::pair<float, unsigned int> &, const std::pair<float, unsigned int> &)>>
            enemyPathCosts(
            [&](const std::pair<float, unsigned int> &costnIndex1, const std::pair<float, unsigned int> &costnIndex2) {
                return costnIndex1.first > costnIndex2.first;
            });

    for (unsigned int i = 0; i < enemies.size(); ++i) {
        const auto &enemy = enemies[i];
        if (!enemy->getDefeated()) {
            auto path = findPath(enemy->getXPos(), enemy->getYPos());
            if (!path.empty()) {
                enemyPathCosts.emplace(costOPath(path), i);
            }
        }
    }

    if (enemyPathCosts.empty()) {
        emit appendto_Command_History(true, "Couldn't find any path to enemy or No enemies left.");
        return;
    }

    float energy = gameWorld->getProtagonistEnergy();
    float health = gameWorld->getProtagonistHealth();

    while (!enemyPathCosts.empty()) {
        auto enemyIndex = enemyPathCosts.top().second;
        auto &enemy = enemies[enemyIndex];
        enemyPathCosts.pop();

        auto path = findPath(enemy->getXPos(), enemy->getYPos());
        float distance = costOPath(path);

        if (distance < energy) {
            if (shouldSurvive) {
                bool hasRequiredHealth =
                        health > enemy->getValue() + ((dynamic_cast<PEnemy *>(enemy.get())) ? 3.0f : 0.0f);

                if (hasRequiredHealth) {
                    gameWorld->setPath(std::move(path));
                    return;
                }
            } else {
                gameWorld->setPath(std::move(path));
                return;
            }
        }
    }
    emit appendto_Command_History(true, "Not enough health or energy to attack any enemies!");
}

bool Controller::findHealth(const QString &metric) {
    const auto &healthPacks = gameWorld->getHealthPacks();

    if (healthPacks.empty()) {
        emit appendto_Command_History(true, "No health pack to consume.");
        return true;
    }

    bool searchBest = isFlagSet(metric, "--best", "-b");
    if (!searchBest && !isFlagSet(metric, "--nearest", "-n")) {
        return false;
    }
    auto compareFn = [&](unsigned int index1, unsigned int index2) {
        if (searchBest) {
            int health = gameWorld->getProtagonistHealth();
            double healthDifference1 = abs((GameWorld::maxHealth - health) - healthPacks[index1]->getValue());
            double healthDifference2 = abs((GameWorld::maxHealth - health) - healthPacks[index2]->getValue());
            return healthDifference1 > healthDifference2;
        } else {
            return costOPath(findPath(healthPacks[index1]->getXPos(), healthPacks[index1]->getYPos())) >
                   costOPath(findPath(healthPacks[index2]->getXPos(), healthPacks[index2]->getYPos()));
        }
    };

    std::priority_queue<unsigned int, std::vector<unsigned int>, decltype(compareFn)> healthIndices(compareFn);

    for (unsigned int i = 0; i < healthPacks.size(); ++i) {
        if ((searchBest || !findPath(healthPacks[i]->getXPos(), healthPacks[i]->getYPos()).empty()) &&
            healthPacks[i]->getValue() != 0) {
            healthIndices.push(i);
        }
    }

    if (healthIndices.empty()) {
        emit appendto_Command_History(true, "Found no suitable health packs.");
        return true;
    }

    int energy = gameWorld->getProtagonistEnergy();
    if (searchBest) {
        while (!healthIndices.empty()) {
            auto &bestHealthPackIndex = healthIndices.top();
            healthIndices.pop();
            const auto &bestHealthPack = healthPacks[bestHealthPackIndex];
            if (setPathIfReachable(bestHealthPack->getXPos(), bestHealthPack->getYPos(),
                                   energy)) {
                emit appendto_Command_History(false, "Consuming health at " +
                                                     QString::number(bestHealthPack->getXPos()) + ", " +
                                                     QString::number(bestHealthPack->getYPos()));
                return true;
            }
        }
        emit appendto_Command_History(true, "Insufficient energy to reach any health pack!");
    } else {
        unsigned int nearestHealthPackIndex = healthIndices.top();
        const auto &bestHealthPack = healthPacks[nearestHealthPackIndex];
        if (costOPath(findPath(bestHealthPack->getXPos(), bestHealthPack->getYPos())) <=
            energy) {
            emit appendto_Command_History(false, "Restoring health at " +
                                                 QString::number(bestHealthPack->getXPos()) + ", " +
                                                 QString::number(bestHealthPack->getYPos()));
            gameWorld->setPath(findPath(bestHealthPack->getXPos(), bestHealthPack->getYPos()));
        } else {
            emit appendto_Command_History(true, "Don't have enough energy to go to the nearest health pack.");
        }
    }
    return true;
}

void Controller::handleAutoMode() {
    if (gameWorld->isRunning() && !isManualMode) {
        const auto &enemies = gameWorld->getEnemies();
        for (const auto &enemy: enemies) {
            if (enemy->getDefeated()) continue;
            int health = gameWorld->getProtagonistHealth();
            bool hasRequiredHealth = health > enemy->getValue() + ((dynamic_cast<PEnemy *>(enemy.get())) ? 3.0f : 0.0f);
            if (hasRequiredHealth) {
                if (setPathIfReachable(enemy->getXPos(), enemy->getYPos(),
                                       gameWorld->getProtagonistEnergy())){
                    emit appendto_Command_History(
                            false, "Attacking enemy at " +
                                   QString::number(enemy->getXPos()) + ", " +
                                   QString::number(enemy->getYPos()));
                    return;
                }
            } else if (health < GameWorld::maxHealth) {
                if (findHealth("-b"))
                    return;
            }
        }

        emit appendto_Command_History(true, "No more moves available!");
        switchManualMode(true);
        isManualMode = true;
    }
}

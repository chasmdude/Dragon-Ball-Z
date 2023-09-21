#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include <QObject>
#include <memory>

#include <bitset>
#include <set>
#include <QTimer>
#include "View/mainview.h"
#include <lib/world.h>

class GameWorld : public QObject {
Q_OBJECT


public:
    GameWorld(MainView *visualisation);

    ~GameWorld();

    void loadWorld(const QString &filename, unsigned int nrOfEnemies, unsigned int nrOfHealthpacks);

    static bool isValidCoordinate(int x, int y, int numCols, int numRows);

    bool isValidCoordinate(int x, int y);

    static double maxEnergy;
    static double maxHealth;
    static int radius;
    static int framesInGameCycle;
    enum class Direction {
        Left,
        Down,
        Up,
        Right
    };
    enum class ProtagonistState {
        POISONED,
        ATTACK,
        HEAL,
        DEAD,
        ON_FIRE
    };

    static constexpr size_t StateBitsCount =
            static_cast<const size_t>(ProtagonistState::ON_FIRE) + 1;

    QPoint getProtagonistLocation() const;

    int getProtagonistEnergy() const;

    int getProtagonistHealth() const;

    const std::vector<std::unique_ptr<Tile>> &getTiles() const;

    bool isReachable(int x, int y) const;

    bool hasGameObject(int x, int y) const;

    const std::unique_ptr<World> &getWorld() const;

    const std::vector<std::unique_ptr<Enemy>> &getEnemies() const;

    const std::vector<std::unique_ptr<Tile>> &getHealthPacks() const;


    bool isRunning() const;


    void setPath(std::vector<std::unique_ptr<Tile>> path);

    void setProtagonistDirection(Direction direction, int count = 1);

signals:

    void consumeHealth(const QPoint gameCoors);

    void becomeWall(const int index);

    void moveDone();

public slots:

    void update();

    void changeRefreshRate(int refreshRate);

    void poisonNeighbourhood(int poisonLevel);

private:
    std::unique_ptr<World> world;
    std::vector<std::unique_ptr<Tile>> tiles;
    std::unique_ptr<Protagonist> protagonist;
    std::map<int, std::bitset<StateBitsCount>> tileStates;
    std::vector<std::unique_ptr<Tile>> path;
    MainView *view;
    std::map<int, int> enemyTiles;
    std::vector<std::unique_ptr<Tile>> poisonedTiles;
    QTimer *gameUpdateTimer;
    std::vector<std::unique_ptr<Enemy>> enemies;
    int prevX, prevY;
    std::map<int, int> healthTiles;
    std::vector<std::unique_ptr<Tile>> healthSupplies;

    int steps = 0;
    Direction protagonistDirection;
    bool isGameRunning = false;

    int tileIndex(int x, int y) const;

    void spawnProtagonist();

    void moveProtagonist();

    void visualiseMap();

    void spawnHealthPacks();

    void spawnEnemies();

    void reset();

    void findCollision();

    void checkGameStatus();

    void updateEffects();
};

#endif

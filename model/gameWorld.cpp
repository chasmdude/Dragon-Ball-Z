#include <iostream>
#include <algorithm>
#include <bitset>
#include <unordered_set>
#include "gameWorld.h"
#include "View/graphical/graphicView.h"
#include "xenemy.h"


double GameWorld::maxEnergy = 100;
double GameWorld::maxHealth = 100;
int GameWorld::radius = 0;
int GameWorld::framesInGameCycle = 4;

constexpr size_t GameWorld::StateBitsCount;

struct PairHash {
    template<class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

int GameWorld::tileIndex(int x, int y) const {
    return y * world->getCols() + x;
}

void GameWorld::spawnProtagonist() {
    int x = protagonist->getXPos();
    int y = protagonist->getYPos();
    int pos = tileIndex(x, y);
    float value = tiles.at(pos)->getValue();
    int shift = 1;
    std::unordered_set<std::pair<int, int>, PairHash> visited;

    while (value == std::numeric_limits<float>::infinity()) {
        for (int dx = -shift; dx <= shift; ++dx) {
            for (int dy = -shift; dy <= shift; ++dy) {
                if (dx == 0 && dy == 0)
                    continue;
                int new_x = x + dx;
                int new_y = y + dy;
                if (isValidCoordinate(new_x, new_y) &&
                    visited.count({new_x, new_y}) == 0) {
                    pos = tileIndex(new_x, new_y);
                    value = tiles.at(pos)->getValue();
                    if (value != std::numeric_limits<float>::infinity()) {
                        x = new_x;
                        y = new_y;
                        break; // Exit inner loop
                    }
                    visited.insert({new_x, new_y});
                }
            }
            if (value != std::numeric_limits<float>::infinity())
                break;
        }
        shift++;
    }

    std::cout << "Protagonist shifted to a reachable tile at (" << x << ", " << y << ")"
              << std::endl;


    protagonist->setPos(x, y);
    prevX = x;
    prevY = y;

    view->updateProtagonist(protagonist);
}

bool GameWorld::isValidCoordinate(int x, int y, int numCols, int numRows) {
    return x >= 0 && x < numCols && y >= 0 && y < numRows;
}

void GameWorld::moveProtagonist() {
    int x = protagonist->getXPos();
    int y = protagonist->getYPos();
    int cols = world->getCols();
    int rows = world->getRows();

    if (tiles.at(cols * y + x)->getValue() == std::numeric_limits<float>::infinity())   // retract back to old pos
    {
        protagonist->setPos(prevX, prevY);
        return;
    } else if (path.empty() && steps > 0)   // move with ASWD
    {
        static const std::array<std::pair<int, int>, 4> directionChanges = {
                {{-1, 0}, {0, 1}, {0, -1}, {1, 0}} // Left, Down, Up, Right
        };

        x = std::clamp(x + directionChanges[static_cast<int>(protagonistDirection)].first, 0, cols - 1);
        y = std::clamp(y + directionChanges[static_cast<int>(protagonistDirection)].second, 0, rows - 1);

        auto &tile = tiles.at(tileIndex(x, y));
        steps--;
    } else if (!path.empty()) {   // move with path
        auto &tile = path.back();
        x = tile->getXPos();
        y = tile->getYPos();
        path.pop_back();

        if (path.empty())   // finished move
                emit moveDone();
    } else
        return;

    auto &tile = tiles.at(tileIndex(x, y));
    float energyCost = 1.0f - tile->getValue();

    if (tile->getValue() == std::numeric_limits<float>::infinity()) {
        if (path.empty()) {
            steps = 0;
        }
        std::cout << "Hit a wall" << std::endl;
    } else if (energyCost > protagonist->getEnergy()) {
        int dx[] = {0, 1, -1};
        int dy[] = {0, 1, -1};
        for (auto dxe: dx) {
            for (auto dye: dy) {
                if (dye == 0 && dxe == 0) {
                    continue;
                }
                auto nx = x + dxe;
                int ny = y + dye;
                int ppos = tileIndex(nx, ny);
                auto &ptile = tiles.at(ppos);
                auto eCost = 1.0f - ptile->getValue();
                if (eCost <= protagonist->getEnergy()) {
                    path.clear();
                    view->appendto_Command_History(true, "Cannot move to this tile, out of energy!");
                    return;
                }
            }
        }
        view->appendto_Command_History(true, "Cannot move to any neighbouring tile, out of energy!");
        protagonist->setEnergy(0);
    } else {
        float newEnergy = protagonist->getEnergy() - energyCost;
        protagonist->setEnergy(newEnergy);
        prevX = protagonist->getXPos();
        prevY = protagonist->getYPos();
        protagonist->setPos(x, y);
    }
}

void GameWorld::visualiseMap() {
    view->renderTiles(tiles, world);
}

void GameWorld::spawnEnemies() {
    if (enemies.empty()) return;
    std::sort(enemies.begin(), enemies.end(), [](auto &enemy1, auto &enemy2) {
        return enemy1->getValue() > enemy2->getValue();
    });
    int temp = 0;
    for (const auto &enemy: enemies) {
        int pos = tileIndex(enemy->getXPos(), enemy->getYPos());
        enemyTiles[pos] = temp;
        temp++;
    }

    int random = rand() % enemies.size();
    unsigned long long iter = 0;
    while (dynamic_cast<PEnemy *> (enemies.at(random).get()) &&
           iter < enemies.size()) {
        random = rand() % enemies.size();
        iter++;
    }

    if (iter < enemies.size()) {
        auto &xEnemyCandidate = enemies.at(random);
        auto xEnemy = new XEnemy(xEnemyCandidate->getXPos(), xEnemyCandidate->getYPos(), xEnemyCandidate->getValue());
        connect(gameUpdateTimer, &QTimer::timeout, xEnemy, &XEnemy::updateFrame);
        enemies.at(random).reset(xEnemy);
    }
    for (auto &enemy: enemies) {
        if (auto pEnemy = dynamic_cast<PEnemy *> (enemy.get())) {
            connect(pEnemy, &PEnemy::poisonLevelUpdated, this, &GameWorld::poisonNeighbourhood);
        }
    }

    view->updateEnemies(enemies);
}

void GameWorld::spawnHealthPacks() {
    if (healthSupplies.empty()) return;

    int counter = 0;
    for (const auto &healthPack: healthSupplies) {
        int pos = tileIndex(healthPack->getXPos(), healthPack->getYPos());
        healthTiles[pos] = counter;
        counter++;
    }

    view->updateHealthPacks(healthSupplies, protagonist);
}

void GameWorld::reset() {
    path.clear();
    protagonist.reset();
    healthSupplies.clear();
    enemies.clear();
    view->resetWorld();
    poisonedTiles.clear();
    tileStates.clear();
    enemyTiles.clear();
    healthTiles.clear();
    tiles.clear();
    isGameRunning = false;
    gameUpdateTimer->stop();
}

bool GameWorld::isValidCoordinate(int x, int y) {
    return isValidCoordinate(x, y, world->getCols(), world->getRows());
}
void GameWorld::findCollision() {
    int protagonistPos = tileIndex(protagonist->getXPos(), protagonist->getYPos());
    std::bitset<StateBitsCount> stateSetBits(tileStates[protagonistPos]);
    std::bitset<StateBitsCount> stateBits;
    if (stateSetBits.test(static_cast<size_t>(ProtagonistState::ON_FIRE))) {
        float newHealth = protagonist->getHealth() - (static_cast<size_t>(ProtagonistState::ON_FIRE) + 1);
        if (newHealth <= 0) {
            newHealth = 0;
            stateBits.set(static_cast<size_t>(ProtagonistState::DEAD));
        }
        protagonist->setHealth(newHealth);
        stateBits.set(static_cast<size_t>(ProtagonistState::ON_FIRE));
    }
    if (stateSetBits.test(static_cast<size_t>(ProtagonistState::POISONED))) {
        float newHealth = protagonist->getHealth() - (static_cast<size_t>(ProtagonistState::POISONED) + 1);
        if (newHealth <= 0) {
            newHealth = 0;
            stateBits.set(static_cast<size_t>(ProtagonistState::DEAD));
        }
        protagonist->setHealth(newHealth);
        stateBits.set(static_cast<size_t>(ProtagonistState::POISONED));
    }
    if (healthTiles.find(protagonistPos) != healthTiles.end()) {
        auto &health = healthSupplies.at(healthTiles[protagonistPos]);
        if (health->getValue() == 0) return; // already consumed

        int newHealth = min(protagonist->getHealth() + health->getValue(), (float) maxHealth);
        protagonist->setHealth(newHealth);
        health->setValue(0);

        view->refreshScene();
        stateBits.set(static_cast<size_t>(ProtagonistState::HEAL));
    } else if (enemyTiles.find(protagonistPos) != enemyTiles.end()) {
        auto &enemy = enemies.at(enemyTiles[protagonistPos]);
        if (!enemy->getDefeated()) {
            if (enemy->getValue() - protagonist->getHealth() < 0) {   // enemy isDead
                protagonist->setEnergy(maxEnergy);
                protagonist->setHealth(protagonist->getHealth() - enemy->getValue());
                enemy->setDefeated(true);
                tiles.at(protagonistPos)->setValue(std::numeric_limits<float>::infinity()); // make tile impassable
                emit becomeWall(protagonistPos);

                if (auto *pEnemy = dynamic_cast<PEnemy *>(enemy.get())) {
                    pEnemy->poison();   // trigger poisoning
                } else {
                    stateBits.set(static_cast<size_t>(ProtagonistState::ATTACK));
                }
            } else {    // protagonist isDead
                protagonist->setHealth(0);
                protagonist->setPos(prevX, prevY);
                stateBits.set(static_cast<size_t>(ProtagonistState::DEAD));
            }
        }

        view->refreshScene();
    }

    if (stateBits.any()) {
        emit view->protagonistEffect(stateBits);

        if (stateBits.test(static_cast<size_t>(ProtagonistState::ATTACK)) ||
            stateBits.test(static_cast<size_t>(ProtagonistState::HEAL)) ||
            stateBits.test(static_cast<size_t>(ProtagonistState::ON_FIRE)) ||
            stateBits.test(static_cast<size_t>(ProtagonistState::POISONED))) {
            emit moveDone();    // damage done / heal, calculate next move
        }
    }
}


void GameWorld::updateEffects() {
    int poisoned = 0;
    for (auto &tile: poisonedTiles) {
        int tilePos = tileIndex(tile->getXPos(), tile->getYPos());

        if (tile->getValue() > 0) {
            int newValue = tile->getValue() - 1;
            tile->setValue(newValue);
            tileStates[tilePos].set(static_cast<size_t>(ProtagonistState::POISONED));
            poisoned++;
        } else {
            tileStates[tilePos].reset(static_cast<size_t>(ProtagonistState::POISONED));
        }
    }
    if (poisoned == 0) {
        poisonedTiles.clear();
    }
}

GameWorld::GameWorld(MainView *visualisation) :
        view{visualisation} {
    world = std::make_unique<World>();
    gameUpdateTimer = new QTimer;
    gameUpdateTimer->setInterval(100);
    connect(gameUpdateTimer, &QTimer::timeout, this, &GameWorld::update);
}

GameWorld::~GameWorld() {
    reset();
    gameUpdateTimer->deleteLater();
    world.reset();
}

void GameWorld::loadWorld(const QString &filename, unsigned int nrOfEnemies, unsigned int nrOfHealthpacks) {
    reset();
    world->createWorld(filename, nrOfEnemies, nrOfHealthpacks);
    enemies = world->getEnemies();
    protagonist = world->getProtagonist();
    tiles = world->getTiles();
    healthSupplies = world->getHealthPacks();

    visualiseMap();
    spawnProtagonist();
    spawnHealthPacks();
    spawnEnemies();

    gameUpdateTimer->start();
    isGameRunning = true;
}

const std::vector<std::unique_ptr<Tile> > &GameWorld::getTiles() const {
    return tiles;
}

void GameWorld::checkGameStatus() {
    if (isGameRunning) {
        if (protagonist->getHealth() <= 0 || protagonist->getEnergy() <= 0) {   // out of health_, energy
            isGameRunning = view->endGame(false);
        } else {
            bool areEnemiesDefeated = std::find_if(enemies.begin(), enemies.end(),
                                                   [](const auto &enemy) { return !enemy->getDefeated(); }) ==
                                      enemies.end();
            if (!enemies.empty() &&
                areEnemiesDefeated) {   // no more enemy left
                isGameRunning = view->endGame(true);
            }
        }
    }
}

bool GameWorld::isReachable(int x, int y) const {
    return tiles.at(tileIndex(x, y))->getValue() != std::numeric_limits<float>::infinity();
}

int GameWorld::getProtagonistHealth() const {
    return protagonist->getHealth();
}

void GameWorld::poisonNeighbourhood(int poisonLevel) {
    if (poisonLevel > 0.0f) {
        PEnemy *pEnemy = (PEnemy *) sender();
        radius++;
        int x_start = pEnemy->getXPos();
        int y_start = pEnemy->getYPos();

        for (int i = -radius; i <= radius; ++i) {
            for (int j = -radius; j <= radius; j++) {
                if (i == 0 && j == 0) {
                    continue;
                }
                int x = x_start + i;
                int y = y_start + j;
                if (isValidCoordinate(x, y, world->getCols(), world->getRows())) {
                    auto is_tile = [&x, &y](std::unique_ptr<Tile> &tile) {
                        return x == tile->getXPos() && y == tile->getYPos();
                    };
                    auto result = std::find_if(poisonedTiles.begin(), poisonedTiles.end(), is_tile);
                    if (result == poisonedTiles.end()) {
                        poisonedTiles.emplace_back(std::make_unique<Tile>(x, y, GameWorld::framesInGameCycle));
                    } else {
                        result->get()->setValue(GameWorld::framesInGameCycle);
                    }
                }
            }
        }
    } else {
        radius = 0;
    }
}

bool GameWorld::isRunning() const {
    return isGameRunning;
}

int GameWorld::getProtagonistEnergy() const {
    return protagonist->getEnergy();
}

QPoint GameWorld::getProtagonistLocation() const {
    return QPoint(protagonist->getXPos(), protagonist->getYPos());
}

bool GameWorld::hasGameObject(int x, int y) const {
/*
    return tileEffects.at(tileIndex(x, y))->type != GameObjectType::NO_OBJECT;
*/
    int pos = tileIndex(x, y);
    return (healthTiles.find(pos) != healthTiles.end()) || (enemyTiles.find(pos) != enemyTiles.end());
}

const std::unique_ptr<World> &GameWorld::getWorld() const {
    return world;
}

const std::vector<std::unique_ptr<Enemy> > &GameWorld::getEnemies() const {
    return enemies;
}

void GameWorld::setProtagonistDirection(Direction direction, int count) {
    protagonistDirection = direction;
    steps = count;
    path.clear();// override pathfinder
    view->clearPath();
    view->switchtoManualMode(true);// interrupt auto mode
}

const std::vector<std::unique_ptr<Tile> > &GameWorld::getHealthPacks() const {
    return healthSupplies;
}

void GameWorld::setPath(std::vector<std::unique_ptr<Tile> > newPath) {
    if (newPath.empty()) return;

    this->path.clear();
    this->path.swap(newPath);

    view->setPath(this->path);
}

void GameWorld::update() {
    if (isGameRunning) {
        updateEffects();
        findCollision();
        moveProtagonist();
        checkGameStatus();
    }
}

void GameWorld::changeRefreshRate(int refreshRate) {
    gameUpdateTimer->setInterval((GraphicalRepresentation::maxRefreshInterval + 1) -
                           (refreshRate * (GraphicalRepresentation::maxRefreshInterval / 100)));
}

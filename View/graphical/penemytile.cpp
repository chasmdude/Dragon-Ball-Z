#include "penemytile.h"
#include "model/gameWorld.h"
#include "View/graphical/graphicView.h"

const QString PEnemyTile::idlePixmapPath = ":/images/enemies/penemy/penemy_idle.png";
const QString PEnemyTile::deadPixmapPath = ":/images/enemies/penemy/penemy_dead.png";
const QString PEnemyTile::poisonAttackPixmapPath = ":/images/enemies/penemy/penemy_attack.png";
const QString PEnemyTile::poisonCloudPixmapPath = ":/images/enemies/penemy/poison.png";


PEnemyTile::PEnemyTile(QPoint gameCoords, int poisonLevel, int cols, int rows) :
        EnemyTile(gameCoords, poisonLevel, idlePixmapPath, deadPixmapPath), cols{cols}, rows{rows} {
    const QPixmap &resizePixmap = GraphicalRepresentation::loadAndResizePixmap(poisonAttackPixmapPath);
    enemy_attack = std::make_unique<QGraphicsPixmapItem>(resizePixmap, this);
    enemy_attack->setParentItem(this);
    enemy_attack->setVisible(false);
}

void PEnemyTile::addPoison(QPoint pos) {
    const QPixmap &resizePixmap = GraphicalRepresentation::loadAndResizePixmap(poisonCloudPixmapPath);
    auto *enemy_poison_cloud = new QGraphicsPixmapItem(resizePixmap, this);
    enemy_poison_cloud->setVisible(true);
    enemy_poison_cloud->setPos(pos);
    enemy_poison_clouds.push_back(enemy_poison_cloud);
}

void PEnemyTile::uponDead() {
    deadCall++;

    if (deadCall == 2) {
        GameWorld::radius = 0;
        isDead = true;
        disconnect(sender(), nullptr, this, nullptr);
    }
}

void PEnemyTile::onAttack(int poisonLevel) {
    power = poisonLevel;
    healthLabel->updateHealth(poisonLevel);
    poisoning = 0;
    poisonRadius++;
    idlePix->setVisible(false);
    enemy_attack->setVisible(true);
}

void PEnemyTile::nextFrame() {
    if (!isDead && poisoning != -1) {
        if (poisoning < 15) // 4 cycles
        {
            poisonSurrounding();
            poisoning++;
        } else {
            removePoison();
            poisoning = -1;
        }
    } else if (isDead) {
        healthLabel->setVisible(false);
        idlePix->setVisible(false);
        enemy_attack->setVisible(false);
        removePoison();
        deadPix->setVisible(true);
        disconnect(sender(), nullptr, this, nullptr);
    }
}

void PEnemyTile::poisonSurrounding() {
    int x_start = location.x();
    int y_start = location.y();// poison surround tiles
    for (int i = -poisonRadius; i <= poisonRadius; ++i) {
        for (int j = -poisonRadius; j <= poisonRadius; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            int x = x_start + i;
            int y = y_start + j;
            if (GameWorld::isValidCoordinate(x, y, cols, rows)) {
                std::pair<int, int> coordinates(x, y);
                if (visitedCoordinates.find(coordinates) == visitedCoordinates.end()) {
                    addPoison(QPoint(i, j) * GraphicalRepresentation::tileScale);
                    visitedCoordinates.insert(coordinates);
                }
            }
        }
    }
    visitedCoordinates.clear();
}

void PEnemyTile::removePoison() {
    for (const auto &poisonCloud: enemy_poison_clouds) {
        poisonCloud->setVisible(false);
    }
    enemy_poison_clouds.clear();
}
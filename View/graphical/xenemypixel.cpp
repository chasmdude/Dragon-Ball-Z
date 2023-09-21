#include "xenemypixel.h"
#include "View/graphical/graphicView.h"

const QString XEnemyTile::XEnemyAttackImage = ":/images/enemies/xenemy/xenemy_powerup.png";
const QString XEnemyTile::XEnemyIdleImage = ":/images/enemies/xenemy/xenemy_idle.png";
const QString XEnemyTile::XEnemyDeadImage = ":/images/enemies/xenemy/xenemy_dead.png";

XEnemyTile::XEnemyTile(QPoint gameCoords, int strength) :
        EnemyTile(gameCoords, strength, XEnemyIdleImage, XEnemyDeadImage) {
    const QPixmap &resizeAttackPixmap = GraphicalRepresentation::loadAndResizePixmap(XEnemyAttackImage);
    enemy_attack = std::make_unique<QGraphicsPixmapItem>(resizeAttackPixmap, this);
    enemy_attack->setVisible(false);
}

void XEnemyTile::uponDead() {
    isDead = true;
    disconnect(sender(), nullptr, this, nullptr);
}

void XEnemyTile::onAttack(bool isInvincible) {
    if (!isDead) {
        fireSpread = isInvincible;
    }
}

void XEnemyTile::updateHealth(int health) {
    healthLabel->updateHealth(health);
}

void XEnemyTile::nextFrame() {
    if (!isDead) {
        if (fireSpread) {
            idlePix->setVisible(false);
            enemy_attack->setVisible(true);
        } else {
            enemy_attack->setVisible(false);
            idlePix->setVisible(true);
        }
    } else {
        healthLabel->setVisible(false);
        enemy_attack->setVisible(false);
        idlePix->setVisible(false);
        deadPix->setVisible(true);
        disconnect(sender(), nullptr, this, nullptr);
    }
}


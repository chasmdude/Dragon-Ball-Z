#include "protagonisttile.h"
#include "model/gameWorld.h"
#include "View/graphical/graphicView.h"

const QString ProtagonistTile::protagonistPrefix = ":/images/Goku/";
const QString ProtagonistTile::imgType = ".png";

const QString ProtagonistTile::idlePixmapPath = ":/images/Goku/goku_idle.png";

const QString ProtagonistTile::walkFrontPrefix = "goku_right_";
const int ProtagonistTile::walkFrontFrames = 2;

const QString ProtagonistTile::walkLeftPrefix = "goku_left_";
const int ProtagonistTile::walkLeftFrames = 2;

const QString ProtagonistTile::walkUpPrefix = "goku_up_";
const int ProtagonistTile::walkUpFrames = 1;

const QString ProtagonistTile::walkDownPrefix = "goku_down_";
const int ProtagonistTile::walkDownFrames = 1;

const QString ProtagonistTile::deadPrefix = "goku_dead_";
const int ProtagonistTile::deadFrames = 1;

const QString ProtagonistTile::attackPrefix = "goku_attack_";
const int ProtagonistTile::attackFrames = 1;

const QString ProtagonistTile::healPrefix = "goku_health_";
const int ProtagonistTile::healFrames = 2;

const QString ProtagonistTile::poisonedPrefix = "goku_poisoned_";
const int ProtagonistTile::poisonedFrames = 2;


ProtagonistTile::ProtagonistTile(int health, QPoint gameCoords)
        : ActorTile(gameCoords) {
    const QPixmap &resizeIdlePixmap = GraphicalRepresentation::loadAndResizePixmap(idlePixmapPath);
    idlePix = std::make_unique<QGraphicsPixmapItem>(resizeIdlePixmap, this);
    initializeAnimation(walkFrontAnimation, walkFrontPrefix, walkFrontFrames);
    initializeAnimation(walkLeftAnimation, walkLeftPrefix, walkLeftFrames);
    initializeAnimation(walkUpAnimation, walkUpPrefix, walkUpFrames);
    initializeAnimation(walkDownAnimation, walkDownPrefix, walkDownFrames);

    initializeAnimation(deadAnimation, deadPrefix, deadFrames);
    initializeAnimation(attackAnimation, attackPrefix, attackFrames);
    initializeAnimation(healAnimation, healPrefix, healFrames);
    initializeAnimation(poisonedAnimation, poisonedPrefix, poisonedFrames);


/*    const QPixmap &resizePoisonedPixmap = GraphicalRepresentation::loadAndResizePixmap(poisonedPixPath);
    idlePoisoned = std::make_unique<QGraphicsPixmapItem>(resizePoisonedPixmap, this);
    idlePoisoned->setVisible(false);*/

    healthLabel = std::make_unique<HealthIndicator>(health, this);

    setZValue(static_cast<qreal>(GraphicalRepresentation::Z_Value::zProtagonist));
    setPos(gameCoords * GraphicalRepresentation::tileScale);
}

void ProtagonistTile::updateFrame() {
    hideAllPictures();
    if (isDead) {
        deadAnimation.at(deadAnimation.size() - 1)->setVisible(true);
        disconnect(sender(), nullptr, this, nullptr);
        return;
    } else {
        if (dead != -1) {
            handleDeadAnimation();
        }
    }
    if (attack != -1) {
        handleAttackAnimation();
    } else if (heal != -1) {
        handleHealAnimation();
    } else if (walking != -1 && poisoned == -1) {
        handleWalkAnimation();
    } else {
        if (poisoned != -1) {
            poisonedAnimation.at((poisoned / GameWorld::framesInGameCycle) % poisonedAnimation.size())->setVisible(
                    true);
            if (++poisoned > poisonGameCycles * poisonedAnimation.size() * GameWorld::framesInGameCycle - 1) {
                poisoned = -1;
            }
        } else {
            idlePix->setVisible(true);
        }
    }
}


GameWorld::Direction ProtagonistTile::getDirection(int x, int y) {
    int gameX = location.x(), gameY = location.y();
    if (x < gameX) {
        return GameWorld::Direction::Left;
    } else if (x > gameX) {
        return GameWorld::Direction::Right;
    } else if (y < gameY) {
        return GameWorld::Direction::Up;
    } else {
        return GameWorld::Direction::Down;
    }
}

void ProtagonistTile::handleWalkAnimation() {
    switch (direction) {
        case GameWorld::Direction::Left: {
            walkLeftAnimation.at((walking / GameWorld::framesInGameCycle) % walkLeftAnimation.size())->setVisible(true);
            if (++walking > walkLeftAnimation.size() * GameWorld::framesInGameCycle - 1) {
                walking = -1;
            }
            break;
        }
        case GameWorld::Direction::Down: {
            walkDownAnimation.at((walking / GameWorld::framesInGameCycle) % walkDownAnimation.size())->setVisible(true);
            if (++walking > walkDownAnimation.size() * GameWorld::framesInGameCycle - 1) {
                walking = -1;
            }
            break;
        }
        case GameWorld::Direction::Up: {
            walkUpAnimation.at((walking / GameWorld::framesInGameCycle) % walkUpAnimation.size())->setVisible(true);
            if (++walking > walkUpAnimation.size() * GameWorld::framesInGameCycle - 1) {
                walking = -1;
            }
            break;
        }
        case GameWorld::Direction::Right: {
            walkFrontAnimation.at((walking / GameWorld::framesInGameCycle) % walkFrontAnimation.size())->setVisible(
                    true);
            if (++walking > walkFrontAnimation.size() * GameWorld::framesInGameCycle - 1) {
                walking = -1;
            }
            break;
        }
    }
}

void ProtagonistTile::handleDeadAnimation() {
    if (dead < deadAnimation.size() * (GameWorld::framesInGameCycle) - 1) {
        // Falling animation
        deadAnimation.at(dead % deadAnimation.size())->setVisible(true);
        dead++;
    } else {
        isDead = true;
    }
    return;
}

void ProtagonistTile::handleHealAnimation() {
    if (heal < healGameCycles * GameWorld::framesInGameCycle - 1) {
        healAnimation.at(heal % healAnimation.size())->setVisible(true);
        heal++;
    } else {
        heal = -1;
    }
}

void ProtagonistTile::handleAttackAnimation() {
    if (attack < AttackGameCycles * GameWorld::framesInGameCycle - 1) {
        // Attack animation
        /*if (attack == 4) {
            direction = !direction;
        }*/
        attackAnimation.at(attack % attackAnimation.size())->setVisible(true);
        /*if (direction) {
            attackAnimation.at(0)->setTransform(leftTransformation());
        }*/
        attack++;
    } else {
        attack = -1;
    }
}

void ProtagonistTile::hideAllPictures() {
    idlePix->setVisible(false);
    for (auto &n: walkFrontAnimation) {
        hideAnimation(n);
    }
    for (auto &n: walkUpAnimation) {
        hideAnimation(n);
    }
    for (auto &n: walkLeftAnimation) {
        hideAnimation(n);
    }
    for (auto &n: walkDownAnimation) {
        hideAnimation(n);
    }

    for (auto &n: deadAnimation) {
        hideAnimation(n);
    }

    for (auto &n: attackAnimation) {
        hideAnimation(n);
    }

    for (auto &n: healAnimation) {
        hideAnimation(n);
    }

/*
    idlePoisoned->setVisible(false);
*/
    for (auto &n: poisonedAnimation) {
        n->setVisible(false);
    }

    for (auto &n: fireAnimation) {
        n->setVisible(false);
    }
}

void ProtagonistTile::hideAnimation(std::unique_ptr<QGraphicsPixmapItem> &n) {
    n->setVisible(false);
    n->resetTransform();
}

void ProtagonistTile::initializeAnimation(std::vector<std::unique_ptr<QGraphicsPixmapItem>> &animation,
                                          const QString &fileNamePrefix, int frameCount) {
    animation.reserve(frameCount);
    for (int i = 1; i <= frameCount; ++i) {
        QString resourcePath = QString(protagonistPrefix) + fileNamePrefix + QString::number(i) + imgType;
        const QPixmap &resizeAnimationPixmap = GraphicalRepresentation::loadAndResizePixmap(resourcePath);
        animation.emplace_back(std::make_unique<QGraphicsPixmapItem>(resizeAnimationPixmap, this));
        hideAnimation(animation.back());
    }
}

void ProtagonistTile::moveProtagonist(int x, int y) {
    direction = getDirection(x, y);
    location = QPoint(x, y);
    setPos(location * GraphicalRepresentation::tileScale);
    walking = 0;
}

void ProtagonistTile::onHealthChange(int health) {
    healthLabel->updateHealth(health);
}

void ProtagonistTile::effect(bitset<GameWorld::StateBitsCount> flags) {
    if (flags.test(static_cast<size_t>(GameWorld::ProtagonistState::POISONED))) {
        poisoned = 0;
    }
    if (flags.test(static_cast<size_t>(GameWorld::ProtagonistState::ON_FIRE))) {
    }
    if (flags.test(static_cast<size_t>(GameWorld::ProtagonistState::HEAL))) {
        heal = 0;
    }
    if (flags.test(static_cast<size_t>(GameWorld::ProtagonistState::ATTACK))) {
        attack = 0;
    }
    if (flags.test(static_cast<size_t>(GameWorld::ProtagonistState::DEAD))) {
        dead = 0;
    }
}



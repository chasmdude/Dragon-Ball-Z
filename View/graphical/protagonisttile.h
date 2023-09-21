#ifndef PROTAGONIST_TILE_H
#define PROTAGONIST_TILE_H

#include <QObject>
#include <QPoint>
#include <QGraphicsItemGroup>
#include <QGraphicsPixmapItem>
#include <memory>
#include "healthindicator.h"
#include "ActorTile.h"
#include <model/gameWorld.h>

using namespace std;

class ProtagonistTile : public ActorTile {
Q_OBJECT
private:
    GameWorld::Direction direction;
    int walking = -1;
    int dead = -1;
    int heal = -1;
    int attack = -1;
    int poisoned = -1;

    std::vector<std::unique_ptr<QGraphicsPixmapItem>> walkAnimation;

    std::vector<std::unique_ptr<QGraphicsPixmapItem>> walkFrontAnimation;
    std::vector<std::unique_ptr<QGraphicsPixmapItem>> walkLeftAnimation;
    std::vector<std::unique_ptr<QGraphicsPixmapItem>> walkUpAnimation;
    std::vector<std::unique_ptr<QGraphicsPixmapItem>> walkDownAnimation;

    std::vector<std::unique_ptr<QGraphicsPixmapItem>> poisonedAnimation;
    std::vector<std::unique_ptr<QGraphicsPixmapItem>> deadAnimation;
    std::vector<std::unique_ptr<QGraphicsPixmapItem>> attackAnimation;
    std::vector<std::unique_ptr<QGraphicsPixmapItem>> healAnimation;
    std::vector<std::unique_ptr<QGraphicsPixmapItem>> fireAnimation;

    void hideAllPictures();

    void handleDeadAnimation();

    void handleAttackAnimation();

    void handleHealAnimation();

    void handleWalkAnimation();

public:
    static constexpr int AttackGameCycles = 8;
    static constexpr int healGameCycles = 8;
    static constexpr int poisonGameCycles = 10;

    // Image Map
    static const QString protagonistPrefix;
    static const QString imgType;
    static const QString idlePixmapPath;
    static const QString poisonedPixPath;

//    static const QString walkPrefix;
    static const QString walkFrontPrefix;
    static const QString deadPrefix;
    static const QString attackPrefix;
    static const QString healPrefix;
    static const QString poisonedPrefix;
    static const QString firePrefix;

    static const int deadFrames;
    static const int attackFrames;
    static const int healFrames;
    static const int poisonedFrames;

    static const int walkFrontFrames;
    static const QString walkLeftPrefix;
    static const int walkLeftFrames;

    static const QString walkUpPrefix;
    static const int walkUpFrames;

    static const QString walkDownPrefix;
    static const int walkDownFrames;

    ProtagonistTile(int health, QPoint gameCoords);

    void hideAnimation(std::unique_ptr<QGraphicsPixmapItem> &n);

    void
    initializeAnimation(std::vector<std::unique_ptr<QGraphicsPixmapItem>> &animation, const QString &fileNamePrefix,
                        int frameCount);

public slots:

    void moveProtagonist(int x, int y);

    void onHealthChange(int currentHealth);

    void updateFrame();

    void effect(bitset<GameWorld::StateBitsCount> flags);

    GameWorld::Direction getDirection(int x, int y);
};

#endif // PROTAGONIST_TILE_H
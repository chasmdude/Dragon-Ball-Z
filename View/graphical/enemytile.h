#ifndef ENEMY_TILE_H
#define ENEMY_TILE_H

#include <QObject>
#include <QGraphicsItemGroup>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include <QFont>

#include "healthindicator.h"
#include "ActorTile.h"

class EnemyTile : public ActorTile {
Q_OBJECT

protected:
    int power;
    std::unique_ptr<QGraphicsPixmapItem> deadPix;
public:
    EnemyTile(QPoint gameCoord, int power,
              QString idleImgPath = ":/images/enemies/enemy/enemy_idle.png",
              QString deadImgPath = ":/images/enemies/enemy/enemy_dead.png");

public slots:

    virtual void uponDead();
};

#endif

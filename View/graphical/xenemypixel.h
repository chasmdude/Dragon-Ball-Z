#ifndef XENEMYTILE_H
#define XENEMYTILE_H

#include <QObject>
#include "enemytile.h"

class XEnemyTile : public EnemyTile {
Q_OBJECT

private:
    std::vector<std::unique_ptr<QGraphicsPixmapItem>> enemy_walking;
    bool fireSpread = false;

    std::unique_ptr<QGraphicsPixmapItem> enemy_attack;
    std::unique_ptr<QGraphicsPixmapItem> enemy_fire;

public:
    XEnemyTile(QPoint gameCoords, int strength);

    static const QString XEnemyIdleImage;

    static const QString XEnemyAttackImage;
    static const QString XEnemyDeadImage;
public slots:

    void uponDead() override;

    void nextFrame();

    void updateHealth(int);

    void onAttack(bool);
};

#endif

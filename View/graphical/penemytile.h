#ifndef P_ENEMY_TILE_H
#define P_ENEMY_TILE_H

#include <QObject>
#include "enemytile.h"
#include<set>

class PEnemyTile : public EnemyTile {
Q_OBJECT

private:
    int poisoning = -1;
    int deadCall = 0;
    int poisonRadius = 0;

    std::unique_ptr<QGraphicsPixmapItem> enemy_attack;
    std::vector<QGraphicsPixmapItem *> enemy_poison_clouds;
    std::set<std::pair<int, int>> visitedCoordinates;
    int rows, cols;
public:
    PEnemyTile(QPoint gameCoord, int poisonLevel, int cols, int rows);

    void addPoison(QPoint pos);

    static const QString idlePixmapPath;
    static const QString deadPixmapPath;
    static const QString poisonAttackPixmapPath;
    static const QString poisonCloudPixmapPath;

public slots:

    void uponDead() override;

    void onAttack(int poisonLevel);

    void nextFrame();

    void poisonSurrounding();

    void removePoison();
};

#endif

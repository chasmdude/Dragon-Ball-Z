#ifndef ASTAR_H
#define ASTAR_H

#include  <lib/world.h>
#include <memory>
#include "node.h"
#include "pathfinder.h"
#include "model/gameWorld.h"
#include <queue>

class AStar : public PathFinder {
Q_OBJECT
public:
    AStar(GameWorld*);
    std::vector<std::shared_ptr<Node>> tileCosts;
    std::vector<std::unique_ptr<Tile>> findPath(QPoint end) override;
public slots:

    void makeUnreachable(int index) override;

private:
    GameWorld *gameWorld;
};

#endif

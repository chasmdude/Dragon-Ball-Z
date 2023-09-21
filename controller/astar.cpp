#include "astar.h"
#include <iostream>

void AStar::makeUnreachable(int index) {
    tileCosts.at(index)->setCost(INFINITY);
}

AStar::AStar(GameWorld *gameWorld) : gameWorld{gameWorld} {
    const auto &tiles = gameWorld->getTiles();

    tileCosts.reserve(tiles.size());

    for (const auto &tile: tiles) {
        auto tempNode = std::make_shared<Node>();

        tempNode->setX(tile->getXPos());
        tempNode->setY(tile->getYPos());
        tempNode->setCost(tile->getValue() == std::numeric_limits<double>::infinity() ?
                          INFINITY : 1 - tile->getValue());
        tempNode->setFinalDistance(INFINITY);
        tempNode->setStartDistance(INFINITY);
        tempNode->setVisited(false);
        tempNode->setParent(nullptr);
        tileCosts.push_back(std::move(tempNode));
    }
}

bool compareNodes(const std::shared_ptr<Node> &current, const std::shared_ptr<Node> &end) {
    return current->getFinalDistance() > end->getFinalDistance();
}

std::vector<std::unique_ptr<Tile>> AStar::findPath(const QPoint end) {
    std::for_each(tileCosts.begin(), tileCosts.end(), [](auto &node) {
        node->setFinalDistance(INFINITY);
        node->setStartDistance(INFINITY);
        node->setVisited(false);
        node->setParent(nullptr);
    });

    QPoint start = gameWorld->getProtagonistLocation();
    auto startNode = tileCosts.at(start.y() * gameWorld->getWorld()->getCols() + start.x());
    auto endNode = tileCosts.at(end.y() * gameWorld->getWorld()->getCols() + end.x());

    startNode->setStartDistance(0);
    startNode->setFinalDistance(
            weight * sqrt(pow(endNode->getX() - startNode->getX(), 2) + pow(endNode->getY() - startNode->getY(), 2)));

    int counter = 0;

    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>,
            decltype(&compareNodes)> unVisited(&compareNodes);
    unVisited.push(startNode);
    std::vector<std::unique_ptr<Tile>> path;

    while (!unVisited.empty() && startNode != endNode) {
        startNode = unVisited.top();
        if (!unVisited.empty() && unVisited.top()->isVisited()) {
            unVisited.pop();
        }
        startNode->setVisited(true);
        for (int i = std::max(startNode->getX() - 1, 0); i <= std::min(startNode->getX() + 1, gameWorld->getWorld()->getCols() - 1); i++) {
            for (int j = std::max(startNode->getY() - 1, 0); j <= std::min(startNode->getY() + 1, gameWorld->getWorld()->getRows() - 1); j++) {
                if (((i == startNode->getX() && j == startNode->getY()) ||
                     ((i == startNode->getX() - 1 && j == startNode->getY() - 1) ||
                      (i == startNode->getX() - 1 && j == startNode->getY() + 1) ||
                      (i == startNode->getX() + 1 && j == startNode->getY() - 1) ||
                      (i == startNode->getX() + 1 && j == startNode->getY() + 1))) ||
                    (i != endNode->getX() && j != endNode->getY() && gameWorld->hasGameObject(i, j))) {
                    continue;
                }

                auto neighbor = tileCosts.at(j * gameWorld->getWorld()->getCols() + i);

                if (neighbor->getCost() > 5) continue;

                double shorterDistance = startNode->getStartDistance() + neighbor->getCost();

                if (shorterDistance < neighbor->getStartDistance()) {
                    counter++;
                    neighbor->setParent(startNode);
                    neighbor->setStartDistance(shorterDistance);
                    double finDist = weight * neighbor->getStartDistance() + ((1 - weight) *
                                                                              sqrt(pow(endNode->getX() -
                                                                                       neighbor->getX(), 2) +
                                                                                   pow(endNode->getY() -
                                                                                       neighbor->getY(), 2)));
                    neighbor->setFinalDistance(
                            finDist);
                    if (!neighbor->isVisited()) {
                        unVisited.push(neighbor);
                    }
                    neighbor->setVisited(true);
                }
            }
        }
    }

    path.reserve(counter);
    auto pNode = endNode.get();
    do {
        std::unique_ptr<Tile> tile = std::make_unique<Tile>(pNode->getX(), pNode->getY(), pNode->getCost());
        path.push_back(std::move(tile));
        pNode = pNode->getParent().get();
    } while (pNode != nullptr && pNode->getParent() != nullptr);
    return path;
}

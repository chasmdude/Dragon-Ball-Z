#ifndef NODE_H
#define NODE_H

#include <memory>
class Node {
public:
    Node() {}

    int getX() const { return x; }

    void setX(int value) { x = value; }

    int getY() const { return y; }

    void setY(int value) { y = value; }

    double getCost() const { return cost; }

    void setCost(double value) { cost = value; }

    bool isVisited() const { return visited; }

    void setVisited(bool value) { visited = value; }

    double getFinalDistance() const { return finaldistance; }

    void setFinalDistance(double value) { finaldistance = value; }

    double getStartDistance() const { return startdistance; }

    void setStartDistance(double value) { startdistance = value; }

    std::shared_ptr<Node> getParent() const { return parent; }

    void setParent(std::shared_ptr<Node> value) { parent = value; }

private:
    int x;
    int y;
    double cost;
    bool visited = false;
    double finaldistance = 0.0;
    double startdistance = 0.0;
    std::shared_ptr<Node> parent = nullptr;
};

#endif // NODE_H

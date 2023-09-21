#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <QObject>
#include <lib/world.h>

class PathFinder : public QObject {
Q_OBJECT

public:
    virtual std::vector<std::unique_ptr<Tile>> findPath(const QPoint end) = 0;

    double weight = 0;

    double getWeight() const;

    void setWeight(double newWeight);

public slots:

    virtual void makeUnreachable(int index) = 0;
};

#endif

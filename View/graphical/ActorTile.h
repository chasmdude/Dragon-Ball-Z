#ifndef ACTORTILE_H
#define ACTORTILE_H

#include <QObject>
#include <QGraphicsItemGroup>
#include "healthindicator.h"

class ActorTile : public QObject, public QGraphicsItemGroup {
Q_OBJECT
protected:
    QPoint location;
    bool isDead{};
    std::unique_ptr<HealthIndicator> healthLabel;
    std::unique_ptr<QGraphicsPixmapItem> idlePix;
public:

    ActorTile(QPoint gameCoords);
};

#endif

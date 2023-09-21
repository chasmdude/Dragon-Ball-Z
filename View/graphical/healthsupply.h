#ifndef HEALTHSUPPLY_H
#define HEALTHSUPPLY_H

#include <QObject>
#include <QGraphicsItemGroup>
#include <QGraphicsPixmapItem>

#include "healthindicator.h"
#include "ActorTile.h"

class HealthSupply : public ActorTile {
Q_OBJECT
public:
    HealthSupply(QPoint gameCoord, int potionQuantity);

    static const QString healthImg;

public slots:

    void consume(int x, int y);
};

#endif

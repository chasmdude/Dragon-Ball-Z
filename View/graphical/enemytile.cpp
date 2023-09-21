#include "enemytile.h"
#include "View/graphical/graphicView.h"

EnemyTile::EnemyTile(QPoint gameCoords, int power,
                     QString idleImgPath, QString deadImgPath) :
        ActorTile(gameCoords), power{power} {
    healthLabel = std::make_unique<HealthIndicator>(power, this);

    const QPixmap &resizeIdlePixmap = GraphicalRepresentation::loadAndResizePixmap(idleImgPath);
    idlePix = std::make_unique<QGraphicsPixmapItem>(resizeIdlePixmap, this);

    const QPixmap &resizeDeadPixmap = GraphicalRepresentation::loadAndResizePixmap(deadImgPath);
    deadPix = std::make_unique<QGraphicsPixmapItem>(resizeDeadPixmap, this);

    deadPix->setVisible(false);

    this->setZValue(static_cast<qreal>(GraphicalRepresentation::Z_Value::zActor));
    this->setPos(gameCoords * GraphicalRepresentation::tileScale);
}

void EnemyTile::uponDead() {
    isDead = true;
    deadPix->setVisible(true);
    healthLabel->setVisible(false);
    idlePix->setVisible(false);
    disconnect(sender(), nullptr, this, nullptr);
}

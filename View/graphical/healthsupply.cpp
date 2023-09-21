#include "healthsupply.h"
#include "View/graphical/graphicView.h"

const QString HealthSupply::healthImg = ":images/Goku/dragon_ball.jpg";

HealthSupply::HealthSupply(QPoint gameCoords, int potionQuantity) : ActorTile(gameCoords) {
    healthLabel = std::make_unique<HealthIndicator>(potionQuantity, this);
    const QPixmap &resizeHealthPixmap = GraphicalRepresentation::loadAndResizePixmap(healthImg);
    idlePix = std::make_unique<QGraphicsPixmapItem>(resizeHealthPixmap, this);
    this->setPos(gameCoords * GraphicalRepresentation::tileScale);
    this->setZValue(static_cast<qreal>(GraphicalRepresentation::Z_Value::zHealth));
}

void HealthSupply::consume(int x, int y) {
    if (location.x() == x && location.y() == y) {
        this->setVisible(false);
        disconnect(sender(), nullptr, this, nullptr);
    }
}

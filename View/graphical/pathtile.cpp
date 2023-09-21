#include "pathtile.h"
#include <QBrush>
#include "View/graphical/graphicView.h"

PathTile::PathTile(QPoint gameCoords)
        : QGraphicsRectItem(gameCoords.x() * GraphicalRepresentation::tileScale,
                            gameCoords.y() * GraphicalRepresentation::tileScale, GraphicalRepresentation::tileScale,
                            GraphicalRepresentation::tileScale) {
    setBrush(QBrush(QColor(240, 131, 58)));
    this->setZValue(static_cast<qreal>(GraphicalRepresentation::Z_Value::zPathTile));
}

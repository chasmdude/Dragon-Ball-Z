#include "background.h"
#include <cmath>
#include <QPixmap>
#include "View/graphical/graphicView.h"
#include <QImage>

namespace {
    QColor getColor(float value) {
        if (value == std::numeric_limits<float>::infinity()) {
            return {0, 0, 0};
        } else {
            QColor color = Qt::blue;
            color.setBlueF(value);
            color.setRedF(value);
            color.setGreenF(value);
            return color;
        }
    }
}

void PlayGround::colorTiles(const std::vector<float> &values) {
    QImage scaledImage = pixmap().toImage();
    int width = scaledImage.width();
    int height = scaledImage.height();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = y * width + x;
            scaledImage.setPixelColor(x, y, getColor(values[index]));
        }
    }
    setPixmap(QPixmap::fromImage(scaledImage));
    tileValues = std::move(values);
}

PlayGround::PlayGround(QPixmap image)
        : QGraphicsPixmapItem{image} {
    setZValue(static_cast<qreal>(GraphicalRepresentation::Z_Value::zPlayground));
    setFlag(QGraphicsItem::ItemIsSelectable, false); // Disable selection
}


void PlayGround::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPoint pis = event->scenePos().toPoint();
        QPointF point = event->scenePos();
        qreal x = point.x();
        int xPos = floor(x / GraphicalRepresentation::tileScale);
        qreal y = point.y();
        int yPos = floor(y / GraphicalRepresentation::tileScale);
        emit backgroundClicked(QPoint(xPos, yPos), tileValues[yPos * pixmap().width() + xPos]);
    }
}
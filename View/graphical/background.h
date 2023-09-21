#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>

class PlayGround : public QObject, public QGraphicsPixmapItem {
Q_OBJECT

public:
    explicit PlayGround(QPixmap image);

    void colorTiles(const std::vector<float> &values);

private:
    std::vector<float> tileValues;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

signals:

    void backgroundClicked(QPoint point, float weight);
};

#endif // PLAYGROUND_H
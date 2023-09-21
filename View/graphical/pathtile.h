#ifndef PATHTILE_H
#define PATHTILE_H

#include <QObject>
#include <QGraphicsRectItem>

class PathTile : public QObject, public QGraphicsRectItem {
Q_OBJECT

public:
    PathTile(QPoint gameCoords);
};

#endif

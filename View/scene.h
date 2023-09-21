#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsScene>
#include <QObject>
#include <lib/world.h>

class MainView;


class ViewInterface : public QGraphicsScene {
Q_OBJECT
public:
    ViewInterface(MainView *mainView) : view{mainView} {};

    virtual void clearPath() = 0;

    virtual void addEnemies(const std::vector<std::unique_ptr<Enemy>> &enemies) = 0;

    virtual void
    addTiles(const std::vector<std::unique_ptr<Tile>> &tiles, const std::unique_ptr<World> &world) = 0;

    virtual ~ViewInterface() = default;

    virtual void addHealthPacks(const std::vector<std::unique_ptr<Tile>> &healthPacks,
                                const std::unique_ptr<Protagonist> &protagonist) = 0;

    virtual void addBackground(QString filename) = 0;

    virtual void addProtagonist(const std::unique_ptr<Protagonist> &protagonist) = 0;

    virtual void showPath(const std::vector<std::unique_ptr<Tile>> &tiles) = 0;

protected:
    MainView *view;
};

#endif

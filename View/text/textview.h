#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include "lib/world.h"
#include "View/scene.h"

class TextView : public ViewInterface {
Q_OBJECT

public:
    explicit TextView(MainView *t);

    ~TextView() override;

    void
    addTiles(const std::vector<std::unique_ptr<Tile>> &tiles, const std::unique_ptr<World> &world) override;

    void addBackground(QString filename) override;

    void addEnemies(const std::vector<std::unique_ptr<Enemy>> &enemies) override;

    void addHealthPacks(const std::vector<std::unique_ptr<Tile>> &healthPacks,
                        const std::unique_ptr<Protagonist> &protagonist) override;

    void addProtagonist(const std::unique_ptr<Protagonist> &protagonist) override;

    void clearPath() override;

    void showPath(const std::vector<std::unique_ptr<Tile>> &path) override;
};

#endif

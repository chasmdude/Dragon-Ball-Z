#include "textview.h"

TextView::~TextView() {

}

TextView::TextView(MainView *t) : ViewInterface(t) {

}

void TextView::addTiles(const std::vector<std::unique_ptr<Tile>> &tiles,
                        const std::unique_ptr<World> &world) {

}

void TextView::addBackground(QString filename) {

}

void TextView::addHealthPacks(const std::vector<std::unique_ptr<Tile>> &healthPacks,
                              const std::unique_ptr<Protagonist> &protagonist) {

}

void TextView::addEnemies(const std::vector<std::unique_ptr<Enemy>> &enemies) {}

void TextView::addProtagonist(const std::unique_ptr<Protagonist> &protagonist) {}

void TextView::clearPath() {}

void TextView::showPath(const std::vector<std::unique_ptr<Tile>> &path) {

}
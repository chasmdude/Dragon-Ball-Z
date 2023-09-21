#include <any>
#include "background.h"
#include "enemytile.h"
#include <functional>
#include "graphicView.h"
#include "healthsupply.h"
#include <memory>
#include "model/gameWorld.h"
#include "model/xenemy.h"
#include "pathtile.h"
#include "penemytile.h"
#include "protagonisttile.h"
#include <QImage>
#include <QPixmap>
#include <QString>
#include <tuple>
#include "View/mainview.h"
#include "lib/world.h"
#include "xenemypixel.h"

QPixmap GraphicalRepresentation::loadAndResizePixmap(const QString &resourcePath) {
    QPixmap originalPixmap(resourcePath);
    originalPixmap.setMask(originalPixmap.createMaskFromColor(Qt::white, Qt::MaskInColor));
    QPixmap resizedPixmap = originalPixmap.scaled(QSize(tileScale, tileScale),
                                                  Qt::IgnoreAspectRatio,
                                                  Qt::SmoothTransformation);

    return resizedPixmap;
}

void GraphicalRepresentation::addBackground(QString filename) {
    QPixmap image(filename);
    worldArea = new PlayGround(image);
    worldArea->setScale(tileScale);
    connect(worldArea, &PlayGround::backgroundClicked, view, &MainView::clickedTile);
}

void GraphicalRepresentation::addTiles(const std::vector<std::unique_ptr<Tile>> &tiles,
                                       const std::unique_ptr<World> &world) {
    vector<float> values;
    for (const auto &tile: tiles) {
        float tileValue = tile->getValue();
        values.push_back(tileValue);
    }
    worldArea->colorTiles(values);
    this->addItem(worldArea);
}

GraphicalRepresentation::GraphicalRepresentation(MainView *v) :
        ViewInterface(v) {
    animationLoop = std::make_unique<QTimer>(this);
    animationLoop->setInterval(50);
    connect(v, &MainView::speedUpdated, this, &GraphicalRepresentation::changeRefreshRate);
    animationLoop->start();
}

GraphicalRepresentation::~GraphicalRepresentation() {
    this->clear();
    animationLoop->deleteLater();
}

void GraphicalRepresentation::addEnemies(const std::vector<std::unique_ptr<Enemy>> &enemies) {
    for (const auto &enemy: enemies) {
        if (const auto *pEnemy = dynamic_cast<PEnemy *> (enemy.get())) {
            auto pEnemyPixel = new PEnemyTile(QPoint(pEnemy->getXPos(),
                                                     pEnemy->getYPos()),
                                              pEnemy->getPoisonLevel(), worldArea->boundingRect().width(),
                                              worldArea->boundingRect().height());

            connect(pEnemy, &PEnemy::poisonLevelUpdated, pEnemyPixel, &PEnemyTile::onAttack);
            connect(animationLoop.get(), &QTimer::timeout, pEnemyPixel, &PEnemyTile::nextFrame);
            connect(pEnemy, &PEnemy::dead, pEnemyPixel, &PEnemyTile::uponDead);
            this->addItem(std::move(pEnemyPixel));
        } else if (const auto *xEnemy = dynamic_cast<XEnemy *> (enemy.get())) {
            auto xEnemyPixel = new XEnemyTile(QPoint(enemy->getXPos(),
                                                     enemy->getYPos()),
                                              enemy->getValue());

            connect(xEnemy, &XEnemy::becameInvincible, xEnemyPixel, &XEnemyTile::onAttack);
            connect(xEnemy, &XEnemy::healthChanged, xEnemyPixel, &XEnemyTile::updateHealth);
            connect(xEnemy, &XEnemy::dead, xEnemyPixel, &XEnemyTile::uponDead);
            connect(animationLoop.get(), &QTimer::timeout, xEnemyPixel, &XEnemyTile::nextFrame);
            this->addItem(std::move(xEnemyPixel));
        } else {
            auto enemyPixel = new EnemyTile(QPoint(enemy->getXPos(),
                                                   enemy->getYPos()),
                                            enemy->getValue());

            connect(enemy.get(), &Enemy::dead, enemyPixel, &EnemyTile::uponDead);
            this->addItem(std::move(enemyPixel));
        }
    }
}

void GraphicalRepresentation::addHealthPacks(const std::vector<std::unique_ptr<Tile>> &healthPacks,
                                             const std::unique_ptr<Protagonist> &protagonist) {
    for (const auto &health: healthPacks) {
        auto healthPixel = new HealthSupply(QPoint(health->getXPos(),
                                                   health->getYPos()),
                                            health->getValue());

        connect(protagonist.get(), &Protagonist::posChanged, healthPixel, &HealthSupply::consume);
        this->addItem(std::move(healthPixel));
    }
}

void GraphicalRepresentation::showPath(const std::vector<std::unique_ptr<Tile> > &pathTiles) {
    clearPath();
    path.reserve(pathTiles.size());
    destPoint = QPoint(pathTiles.at(0)->getXPos(), pathTiles.at(0)->getYPos());
    for (const auto &tile: pathTiles) {
        auto pathTile = std::make_unique<PathTile>(QPoint(tile->getXPos(), tile->getYPos()));
        this->addItem(pathTile.get());
        path.push_back(std::move(pathTile));
    }
}

void GraphicalRepresentation::addProtagonist(const std::unique_ptr<Protagonist> &protagonist) {
    auto protagonistPixel = new ProtagonistTile(protagonist->getHealth(),
                                                QPoint(protagonist->getXPos(),
                                                       protagonist->getYPos()));

    connect(protagonist.get(), &Protagonist::posChanged, protagonistPixel, &ProtagonistTile::moveProtagonist);
    connect(protagonist.get(), &Protagonist::healthChanged, protagonistPixel, &ProtagonistTile::onHealthChange);
    connect(protagonist.get(), &Protagonist::posChanged, this, &GraphicalRepresentation::protagonistMoved);
    connect(view, &MainView::protagonistEffect, protagonistPixel, &ProtagonistTile::effect);
    connect(animationLoop.get(), &QTimer::timeout, protagonistPixel, &ProtagonistTile::updateFrame);
    this->addItem(std::move(protagonistPixel));
}

void GraphicalRepresentation::changeRefreshRate(int refreshRate) {
    animationLoop->setInterval((GraphicalRepresentation::maxRefreshInterval+1)-(refreshRate*(GraphicalRepresentation::maxRefreshInterval/100))/ GameWorld::framesInGameCycle);
}

void GraphicalRepresentation::protagonistMoved(int x, int y) {
    if (destPoint.x()==x && y == destPoint.y()) {
        destPoint.setX(-1);
        clearPath();
        destPoint.setY(-1);
    }
    if (!path.empty())
        path.pop_back();
}

void GraphicalRepresentation::clearPath() {
    path.clear();
}

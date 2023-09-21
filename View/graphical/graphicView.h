#ifndef GRAPHICVIEW_H
#define GRAPHICVIEW_H

#include "background.h"
#include "pathtile.h"
#include <QTimer>
#include "View/scene.h"

class GraphicalRepresentation : public ViewInterface {
Q_OBJECT
public:
    static constexpr int tileScale = 56;
    static constexpr int maxRefreshInterval = 200;
    enum class Z_Value {
        zPlayground,
        zPathTile,
        zHealth,
        zActor,
        zProtagonist
    };

    GraphicalRepresentation(MainView *v);

    ~GraphicalRepresentation() override;

    void addBackground(QString filename) override;

    void
    addTiles(const std::vector<std::unique_ptr<Tile>> &tiles, const std::unique_ptr<World> &world) override;

    void addEnemies(const std::vector<std::unique_ptr<Enemy>> &enemies) override;

    void addHealthPacks(const std::vector<std::unique_ptr<Tile>> &healthPacks,
                        const std::unique_ptr<Protagonist> &protagonist) override;

    void addProtagonist(const std::unique_ptr<Protagonist> &protagonist) override;

    void clearPath() override;

    static QPixmap loadAndResizePixmap(const QString &resourcePath);

    void showPath(const std::vector<std::unique_ptr<Tile> > &pathTiles) override;

public slots:

    void changeRefreshRate(int refreshRate);

    void protagonistMoved(int x, int y);

private:
    PlayGround *worldArea;
    std::unique_ptr<QTimer> animationLoop;
    QPoint destPoint;
    std::vector<std::unique_ptr<PathTile>> path;
};

#endif

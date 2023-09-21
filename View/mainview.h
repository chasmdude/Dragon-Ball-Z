#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "scene.h"
//#include "model/gameWorld.h"

#include <QMainWindow>
#include <QGraphicsView>
#include <QPushButton>
#include <QLineEdit>
#include <QInputDialog>
#include <QEvent>
#include <QKeyEvent>
#include <QScrollBar>

#include <bitset>
#include <memory>

#include <lib/world.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainView; }
QT_END_NAMESPACE

using namespace std;

class MainView : public QMainWindow {
Q_OBJECT

public:
    explicit MainView(QWidget *parent = nullptr);

    static const QString showTerminal;
    static const QString showSettings;

    ~MainView() override;

    bool eventFilter(QObject *object, QEvent *event) override;

    void resetWorld();

    enum class SceneView {
        Graphical_2D,
        Text
    };

    void renderTiles(const std::vector<std::unique_ptr<Tile>> &tile, const std::unique_ptr<World> &world);

    void updateProtagonist(const std::unique_ptr<Protagonist> &protagonist);

    void clearPath();

    void updateHealthPacks(const std::vector<std::unique_ptr<Tile>> &healthPacks,
                           const std::unique_ptr<Protagonist> &protagonist);

    void updateEnemies(const std::vector<std::unique_ptr<Enemy>> &enemies);

    bool endGame(bool win);

    void refreshScene();

    void setPath(const std::vector<std::unique_ptr<Tile>> &tiles);

public slots:

    void selectMap(QString path);

    void on_loadWorld_clicked();

    void commandEntered();

    void setCommand(QString string);

    void appendto_Command_History(bool critical, QString string);

    void clearHistory();

    void closeTerminal();

    void clickedTile(QPoint point, float value);

    void uponProtagonistPosChanged(int x, int y);

    void onHealthChanged(int health);

    void onEnergyChanged(int energy);

    void switchtoManualMode(bool manualMode);

private slots:
    void on_Auto_Mode_triggered(bool checked);

    void on_zoomSpinBox_valueChanged(int val);

    void on_terminal_triggered();

    void on_textView_triggered(bool checked);

    void on_graphicView_triggered(bool checked);

    void on_actionManual_triggered(bool checked);

    void on_speedSpin_editingFinished();

    void on_edit_settings_triggered();

    void on_speedSlider_valueChanged(int value);

    void on_zoomSlider_valueChanged(int value);

    void on_resetButton_clicked();

    void on_speedSpin_valueChanged(int val);

    void on_weightSpin_editingFinished();

    void on_weightSlider_valueChanged(int value);

    void on_closeButton_clicked();

    void on_weightSpin_valueChanged(double arg1);

    void on_buttonOpenWorld_clicked();

    void on_actionToggle_Centre_triggered(bool checked);

    void on_zoomSpinBox_editingFinished();

signals:
    void switchManualMode(bool manualMode);

    void zoomChanged(float zoom);

    void cmdEntered(QString command);

    void worldSelected(const QString fileName, int enemies, int healthPacks);

    void weightHeuristicChanged(float weight);

    void suggestFillCommand(const QString command);

    void speedUpdated(int refreshRate);

    void protagonistEffect(std::bitset<5> flag);

private:
    Ui::MainView *mainUI;
    std::vector<std::unique_ptr<ViewInterface>> scenes;
    bool isGameRunning;
    float zoom = 1.0f;
    QPoint protagonistPos;
    QString chosenWorld;

    void hideToolWindows();

    void switchScene(SceneView sceneView);

    void protagonistMoved(QPoint protagonistPos);

    void initPlayGround();

    void populateViews();
};

#endif

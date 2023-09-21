#include "View/mainview.h"
#include "controller/controller.h"
#include "model/gameWorld.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication q_Application(argc, argv);
    std::shared_ptr<MainView> mainView = std::make_shared<MainView>();
    mainView->setWindowTitle("Dragon ball Z");
    std::shared_ptr<GameWorld> game_world = std::make_shared<GameWorld>(mainView.get());
    std::shared_ptr<Controller> gameController = std::make_shared<Controller>(game_world.get());
    q_Application.installEventFilter(mainView.get());

    QObject::connect(mainView.get(), &MainView::worldSelected, gameController.get(), &Controller::loadWorld);
    QObject::connect(mainView.get(), &MainView::switchManualMode, gameController.get(), &Controller::switchManualMode);
    QObject::connect(mainView.get(), &MainView::cmdEntered, gameController.get(), &Controller::commandInvoke);
    QObject::connect(mainView.get(), &MainView::suggestFillCommand, gameController.get(), &Controller::suggest_commands);
    QObject::connect(mainView.get(), &MainView::weightHeuristicChanged, gameController.get(), &Controller::changeWeight);

    QObject::connect(mainView.get(), &MainView::speedUpdated, game_world.get(), &GameWorld::changeRefreshRate);

    QObject::connect(gameController.get(), &Controller::appendto_Command_History, mainView.get(),
                     &MainView::appendto_Command_History);
    QObject::connect(gameController.get(), &Controller::clearHistory, mainView.get(), &MainView::clearHistory);
    QObject::connect(gameController.get(), &Controller::closeTerminal, mainView.get(), &MainView::closeTerminal);
    QObject::connect(gameController.get(), &Controller::setCommand, mainView.get(), &MainView::setCommand);
    QObject::connect(game_world.get(), &GameWorld::moveDone, gameController.get(), &Controller::handleAutoMode);

    mainView->show();
    mainView->selectMap(":/images/maps/worldmap.png");
    mainView->on_loadWorld_clicked();
    return q_Application.exec();
}

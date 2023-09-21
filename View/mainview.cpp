#include "mainview.h"
#include "ui_mainview.h"
#include "graphical/graphicView.h"
#include "View/text/textview.h"

#include <QMessageBox>
#include <model/gameWorld.h>
#include <iostream>
#include <QFileDialog>

const QString MainView::showSettings = "Edit Settings";
const QString MainView::showTerminal = "Show Terminal";


QT_BEGIN_NAMESPACE
namespace Ui { class MainView; }
QT_END_NAMESPACE

MainView::MainView(QWidget *parent) :
        QMainWindow(parent),
        mainUI(new Ui::MainView) {
    mainUI->setupUi(this);
    populateViews();
    mainUI->terminal->text() = showTerminal;
    hideToolWindows();
    initPlayGround();
}

MainView::~MainView() {
    delete mainUI;
    scenes.clear();
}

void MainView::hideToolWindows() {
    mainUI->cmdHistory->hide();
    mainUI->input->hide();
    mainUI->input->setFocusPolicy(Qt::NoFocus);
    mainUI->actionManual->setEnabled(false);
    mainUI->settingsBox->hide();
    mainUI->graphicView->setEnabled(false);
}

void MainView::populateViews() {
    scenes.reserve(2);
    auto graphics = make_unique<GraphicalRepresentation>(this);
    scenes.push_back(std::move(graphics));
    auto text = make_unique<TextView>(this);
    scenes.push_back(std::move(text));
}

void MainView::initPlayGround() {
    mainUI->playGround->setScene(scenes.at(0).get());
    mainUI->playGround->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mainUI->playGround->scale(zoom, zoom);
    mainUI->playGround->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mainUI->playGround->verticalScrollBar()->setEnabled(true);
    mainUI->playGround->horizontalScrollBar()->setEnabled(true);
}

bool MainView::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        if (mainUI->input->hasFocus()) {
            auto *keyEvent = dynamic_cast<QKeyEvent *> (event);
            auto key = (Qt::Key) keyEvent->key();

            if (key == Qt::Key_Tab) {
                emit suggestFillCommand(mainUI->input->text());
                return true;
            }
        } else {
            auto *keyEvent = dynamic_cast<QKeyEvent *> (event);
            auto key = (Qt::Key) keyEvent->key();
            if (keyEvent->modifiers() == Qt::ControlModifier) {
                if (key == Qt::Key_T) {
                    on_terminal_triggered();
                } else if (key == Qt::Key_S) {
                    on_edit_settings_triggered();
                }
            }
            if (isGameRunning && mainUI->playGround->hasFocus()) {
                if (key == Qt::Key_Left ||  key == Qt::Key_A) {
                    emit cmdEntered("left");
                    return true;
                } else if ( key == Qt::Key_Down||key == Qt::Key_S) {
                    emit cmdEntered("down");
                    return true;
                } else if ( key == Qt::Key_Up || key == Qt::Key_W) {
                    emit cmdEntered("up");
                    return true;
                } else if ( key == Qt::Key_Right || key == Qt::Key_D) {
                    emit cmdEntered("right");
                    return true;
                }
            }
        }
    } else if (event->type() == QEvent::Wheel) {
        auto *wheelEvent = dynamic_cast<QWheelEvent *>(event);
        if (wheelEvent->modifiers() & Qt::ControlModifier) {
            auto angleDelta = wheelEvent->angleDelta();
            int zoom_step_size = 5;
            if (angleDelta.y() > 0) {
                on_zoomSlider_valueChanged(min(zoom * 100 + zoom_step_size, float(200)));
            } else {
                on_zoomSlider_valueChanged(max(zoom * 100 - zoom_step_size, float(1)));
            }
            return true;
        }
    }
    return QObject::eventFilter(object, event);
}

void MainView::resetWorld() {
    on_resetButton_clicked();
    mainUI->playGround->resetTransform();
    mainUI->cmdHistory->clear();
    onHealthChanged(GameWorld::maxHealth);
    onEnergyChanged(GameWorld::maxEnergy);

    for (auto &scene: scenes) {
        scene->clearPath();
        scene->clear();
    }
}

void MainView::updateHealthPacks(const std::vector<std::unique_ptr<Tile> > &healthPacks,
                                 const std::unique_ptr<Protagonist> &protagonist) {
    for (const auto &scene: scenes) {
        scene->addHealthPacks(healthPacks, protagonist);
    }
}

void MainView::refreshScene() {
    mainUI->playGround->scene()->update();
}

void MainView::updateEnemies(const std::vector<std::unique_ptr<Enemy> > &enemies) {
    for (const auto &scene: scenes) {
        scene->addEnemies(enemies);
    }
}

void MainView::updateProtagonist(const std::unique_ptr<Protagonist> &protagonist) {
    isGameRunning = true;
    connect(protagonist.get(), &Protagonist::healthChanged, this, &MainView::onHealthChanged);
    connect(protagonist.get(), &Protagonist::energyChanged, this, &MainView::onEnergyChanged);
    connect(protagonist.get(), &Protagonist::posChanged, this, &MainView::uponProtagonistPosChanged);

    uponProtagonistPosChanged(protagonist->getXPos(), protagonist->getYPos());

    for (auto &scene: scenes) {
        scene->addProtagonist(protagonist);
    }
}

void MainView::setPath(const std::vector<std::unique_ptr<Tile> > &tiles) {
    for (const auto &scene: scenes) {
        scene->showPath(tiles);
    }
    refreshScene();
}

void MainView::clearPath() {
    for (auto &scene: scenes)
        scene->clearPath();

    refreshScene();
}

bool MainView::endGame(bool win) {
    isGameRunning = false;
    clearPath();

    QString message;
    QString title;

    if (win) {
        title = "Winner!";
        message = "You won!";
    } else {
        title = "Loser!";
        message = "You lose!";
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, title, message + " Do you want to play again?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        on_loadWorld_clicked();
        return true;
    } else {
        return false;
    }
}

void MainView::renderTiles(const std::vector<std::unique_ptr<Tile>> &tile, const std::unique_ptr<World> &world) {
    for (const auto &scene: scenes) {
        scene->addTiles(tile, world);
    }
}

void MainView::on_terminal_triggered() {
    if (mainUI->terminal->text() == showTerminal) {
        mainUI->terminal->setText("Hide Terminal");
        if (mainUI->edit_settings->text() != showSettings) {
            on_edit_settings_triggered();
        }
        mainUI->input->setFocusPolicy(Qt::ClickFocus);
        mainUI->input->show();
        mainUI->input->setFocus();
        mainUI->cmdHistory->show();
    } else {
        mainUI->terminal->setText("Show Terminal");
        mainUI->cmdHistory->hide();
        mainUI->input->hide();
        mainUI->input->setFocusPolicy(Qt::NoFocus);
        mainUI->playGround->setFocus();
    }
    protagonistMoved(protagonistPos);
}


void MainView::on_edit_settings_triggered() {
    if (mainUI->edit_settings->text() == showSettings) {
        mainUI->edit_settings->setText("Hide Settings");
        if (mainUI->terminal->text() != showTerminal) {
            on_terminal_triggered();
        }
        mainUI->settingsBox->show();
    } else {
        mainUI->edit_settings->setText(showSettings);
        mainUI->settingsBox->hide();
    }
    protagonistMoved(protagonistPos);
}


void MainView::on_graphicView_triggered(bool checked) {
    if (checked) switchScene(SceneView::Graphical_2D);
}


void MainView::on_textView_triggered(bool checked) {
    if (checked) {
        switchScene(SceneView::Text);
    }
}


void MainView::on_actionManual_triggered(bool checked) {
    switchtoManualMode(checked);
}

void MainView::on_Auto_Mode_triggered(bool checked) {
    switchtoManualMode(!checked);
}

void MainView::switchScene(SceneView sceneView) {
    switch (sceneView) {
        case SceneView::Graphical_2D: {
            mainUI->graphicView->setChecked(true);
            mainUI->graphicView->setEnabled(false);
            mainUI->textView->setEnabled(true);
            mainUI->textView->setChecked(false);
            if (mainUI->terminal->text() != showTerminal) {
                on_terminal_triggered();
            }
            break;
        }
        case SceneView::Text: {
            mainUI->graphicView->setChecked(false);
            mainUI->graphicView->setEnabled(true);
            mainUI->textView->setChecked(true);
            mainUI->textView->setEnabled(false);
            if (mainUI->terminal->text() == showTerminal) {
                on_terminal_triggered();
            }
        }
    }
    mainUI->playGround->setScene(scenes.at(static_cast<unsigned long>(sceneView)).get());
}

void MainView::protagonistMoved(QPoint newProtagonistPos) {
    protagonistPos = newProtagonistPos;
    if (mainUI->actionToggle_Centre->isChecked()) {
        mainUI->playGround->centerOn(newProtagonistPos);
    }
    refreshScene();
}

void MainView::onHealthChanged(int health) {
    mainUI->healthBar->setValue(health);
    mainUI->healthLabel->setText(QString::number(health));
}

void MainView::commandEntered() {
    QString command = mainUI->input->text();
    appendto_Command_History(false, command);
    mainUI->input->clear();
    emit cmdEntered(command);
}

void MainView::onEnergyChanged(int energy) {
    mainUI->energyBar->setValue(energy);
    mainUI->energyLabel->setText(QString::number(energy));
}

void MainView::switchtoManualMode(bool manualMode) {
    mainUI->actionManual->setChecked(manualMode);
    mainUI->actionManual->setEnabled(!manualMode);
    mainUI->Auto_Mode->setChecked(!manualMode);
    mainUI->Auto_Mode->setEnabled(manualMode);
    emit switchManualMode(manualMode);
}

void MainView::appendto_Command_History(bool critical, QString string) {
    if (critical) {
        QMessageBox::information(this, "Info", string);
    }
    mainUI->cmdHistory->append(string);
}

void MainView::setCommand(QString string) {
    mainUI->input->setText(string);
}

void MainView::clearHistory() {
    mainUI->cmdHistory->clear();
}

void MainView::on_zoomSpinBox_valueChanged(int val) {
    mainUI->zoomSlider->setValue(val);
    mainUI->playGround->resetTransform();
    zoom = val / 100.0f;
    mainUI->playGround->scale(zoom, zoom);
    protagonistMoved(protagonistPos);
    emit zoomChanged(zoom);
}

void MainView::closeTerminal() {
    mainUI->terminal->text() = "";
    on_terminal_triggered();
}

void MainView::uponProtagonistPosChanged(int x, int y) {
    protagonistMoved(QPoint(x, y) * GraphicalRepresentation::tileScale);
}

void MainView::on_zoomSlider_valueChanged(int value) {
    mainUI->zoomSpinBox->setValue(value);
}

void MainView::on_weightSlider_valueChanged(int value) {
    mainUI->weightSpin->setValue(value);
    emit weightHeuristicChanged(value);
}

void MainView::clickedTile(QPoint point, float value) {
    if (isGameRunning) {
        QString result;
        if (value != std::numeric_limits<float>::infinity()) {
            result = "Moving to " + QString::number(point.x()) + ", "
                     + QString::number(point.y());
            emit cmdEntered(
                    QString("goto " + QString::number(point.x()) + " " + QString::number(point.y())));
        }
        switchtoManualMode(true);
    }
}

void MainView::on_weightSpin_editingFinished() {
    this->setFocus();
}


void MainView::on_speedSpin_valueChanged(int val) {
    mainUI->speedSlider->setValue(val);
}

void MainView::selectMap(QString path) {
    chosenWorld = path;
    mainUI->labelSelectedMap->setText("World Path: " + path);
}

void MainView::on_speedSlider_valueChanged(int value) {
    mainUI->speedSpin->setValue(value);
    emit speedUpdated(value);
}

void MainView::on_resetButton_clicked() {
    mainUI->weightSlider->setValue(0);
    mainUI->spinHealthPacks->setValue(20);
    mainUI->spinEnemies->setValue(20);
    mainUI->speedSlider->setValue(50);
    mainUI->zoomSlider->setValue(100);
}

void MainView::on_speedSpin_editingFinished() {
    this->setFocus();
}

void MainView::on_closeButton_clicked() {
    on_edit_settings_triggered();
}

void MainView::on_actionToggle_Centre_triggered(bool center_on) {
    protagonistMoved(protagonistPos);
}

void MainView::on_weightSpin_valueChanged(double arg1) {
    mainUI->weightSlider->setValue(arg1);
}

void MainView::on_buttonOpenWorld_clicked() {
    const QString file = QFileDialog::getOpenFileName(this,
                                                      "Open map",
                                                      "",
                                                      "Image Files (*.png *.jpg *.bmp)");
    if (!file.isEmpty())
        selectMap(file);
}

void MainView::on_loadWorld_clicked() {
    if (chosenWorld != "") {
        if (mainUI->edit_settings->text() != showSettings) {
            on_edit_settings_triggered();
        }
        for (const auto &scene: scenes) {
            scene->addBackground(chosenWorld);
        }
        emit worldSelected(chosenWorld, mainUI->spinEnemies->value(), mainUI->spinHealthPacks->value());
    }
}

void MainView::on_zoomSpinBox_editingFinished() {
    this->setFocus();
}


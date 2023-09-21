#ifndef XENEMY_H
#define XENEMY_H

#include <QObject>
#include <random>
#include <lib/world.h>

class GameWorld;

class XEnemy : public Enemy {
Q_OBJECT

public:
    XEnemy(int xPosition, int yPosition, float strength);
    static constexpr double INVINCIBLE_PROBABILITY = 0.25;
    static constexpr int INVINCIBLE_CYCLES = 5;

public slots:
    void updateFrame();
signals:
    void becameInvincible(bool);
    void healthChanged(int);

private:
    int loopCounter = 0;
    std::random_device randomDevice;
    std::mt19937 gen;
    void setValue(float newValue) { value = newValue; emit healthChanged(value); };
};

#endif // XENEMY_H

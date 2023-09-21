#include "xenemy.h"
#include "gameWorld.h"

constexpr double XEnemy::INVINCIBLE_PROBABILITY;
constexpr int XEnemy::INVINCIBLE_CYCLES;

XEnemy::XEnemy(int xPosition, int yPosition, float strength) :
        Enemy(xPosition, yPosition, strength), randomDevice(), gen(randomDevice()) {
}

void XEnemy::updateFrame() {
    if (getDefeated()) {
        disconnect(sender(), nullptr, this, nullptr);
    }
    static std::uniform_real_distribution<double> dis(0.0, 1.0);
    if (loopCounter == 0) {
        double randomValue = dis(gen);
        if (randomValue < INVINCIBLE_PROBABILITY) {
            emit becameInvincible(true);
            setValue(GameWorld::maxHealth+20);
        } else {
            static uniform_real_distribution<double> distribution = std::uniform_real_distribution<double>(60.0, 99);
            emit becameInvincible(false);
            setValue(distribution(gen));
        }
    } else if (loopCounter >= INVINCIBLE_CYCLES * GameWorld::framesInGameCycle) {
        loopCounter = -1;
    }
    loopCounter++;
}


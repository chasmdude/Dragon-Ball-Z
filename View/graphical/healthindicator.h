#ifndef HEALTH_INDICATOR_H
#define HEALTH_INDICATOR_H

#include <QGraphicsTextItem>
#include <QColor>

class HealthIndicator : public QGraphicsTextItem {
public:
    explicit HealthIndicator(int maxHealth, QGraphicsItem *parent = nullptr);

    void updateHealth(int currentHealth);

private:
    int maxHealth;
    int currentHealth_;

    void updateColor();

    static QColor getColorForPercent(qreal percent);
};

#endif
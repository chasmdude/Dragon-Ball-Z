#include "healthindicator.h"
#include "View/graphical/graphicView.h"

HealthIndicator::HealthIndicator(int maxHealth, QGraphicsItem *parent)
        : QGraphicsTextItem(parent), maxHealth(maxHealth), currentHealth_{maxHealth} {
    setDefaultTextColor(QColor(255, 140, 0)); // Dark Orange
    setPos(GraphicalRepresentation::tileScale / 4,
           -(GraphicalRepresentation::tileScale / 2.0)); // add to the tile above it
    setFont(QFont("Roboto", 0.4 * GraphicalRepresentation::tileScale, QFont::Bold));
    setPlainText(QString::number(maxHealth));
}

void HealthIndicator::updateHealth(int currentHealth) {
    currentHealth_ = currentHealth;
    setPlainText(QString::number(currentHealth));
    updateColor();
}

void HealthIndicator::updateColor() {
    qreal percent = static_cast<qreal>(currentHealth_) / maxHealth;
    QColor color = getColorForPercent(percent);
    setDefaultTextColor(color);
}

QColor HealthIndicator::getColorForPercent(qreal percent) {
    if (percent > 1) {
        return {255, 0, 255};
    }
    QColor startColor = QColor(255, 0, 0);     // Red
    QColor endColor = QColor(255, 140, 0); // Dark Orange


    int r = startColor.red() + percent * (endColor.red() - startColor.red());
    int g = startColor.green() + percent * (endColor.green() - startColor.green());
    int b = startColor.blue() + percent * (endColor.blue() - startColor.blue());

    return {r, g, b};
}

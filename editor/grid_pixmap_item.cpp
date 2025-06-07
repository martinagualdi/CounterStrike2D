#include "grid_pixmap_item.h"
#include <QVariant>

GridPixmapItem::GridPixmapItem(const QPixmap& pixmap, int gridSize, QGraphicsItem* parent)
    : QGraphicsPixmapItem(pixmap, parent), gridSize(gridSize) {
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
}

QVariant GridPixmapItem::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == QGraphicsItem::ItemPositionChange) {
        QPointF newPos = value.toPointF();
        int x = static_cast<int>(newPos.x()) / gridSize * gridSize;
        int y = static_cast<int>(newPos.y()) / gridSize * gridSize;
        return QPointF(x, y);
    }
    return QGraphicsPixmapItem::itemChange(change, value);
}

void GridPixmapItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    setOpacity(0.5);
    QGraphicsPixmapItem::mousePressEvent(event);
}

void GridPixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    setOpacity(1.0);
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}
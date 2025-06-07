#ifndef GRID_PIXMAP_ITEM_H
#define GRID_PIXMAP_ITEM_H

#include <QGraphicsPixmapItem>

class GridPixmapItem : public QGraphicsPixmapItem {
public:
    GridPixmapItem(const QPixmap& pixmap, int gridSize, QGraphicsItem* parent = nullptr);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    int gridSize;
    QGraphicsPixmapItem* sombra = nullptr;
};

#endif // GRID_PIXMAP_ITEM_H

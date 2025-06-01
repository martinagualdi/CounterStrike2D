#pragma once
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

class ZonaRectItem : public QGraphicsRectItem {
public:
    ZonaRectItem(const QRectF& rect, const QString& tipo, QGraphicsItem* parent = nullptr);
    void setTexto(const QString& texto);
    void setColor(const QColor& color);
    QString getTipo() const { return tipo; }

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    QGraphicsTextItem* label;
    QString tipo;
    bool resizing = false;
    QPointF resizeStartPos;
    QRectF originalRect;
    const qreal resizeMargin = 10.0;
};

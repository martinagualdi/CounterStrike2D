#ifndef ZONA_RECT_ITEM_H
#define ZONA_RECT_ITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QObject>
#include <QUuid>

class ZonaRectItem : public QObject, public QGraphicsRectItem {
    Q_OBJECT 
public:
    ZonaRectItem(const QRectF& rect, const QString& tipo, QUuid id, QGraphicsItem* parent = nullptr);
    void setTexto(const QString& texto);
    void setColor(const QColor& color);
    QString getTipo() const { return tipo; }
    QRectF oldRect() const { return originalRect; }
    QUuid getId() const {return id;}

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    QGraphicsTextItem* label;
    QString tipo;
    bool resizing = false;
    QPointF resizeStartPos;
    QRectF originalRect;
    const qreal resizeMargin = 10.0;
    QUuid id;
signals:
    void tipoZonaCambiado(ZonaRectItem* zona, const QString& nuevoTipo);
    void zonaRectActualizado(ZonaRectItem* zona);


};

#endif

#include "zona_rect_item.h"
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QGraphicsScene>
#include <QInputDialog>

ZonaRectItem::ZonaRectItem(const QRectF& rect, const QString& tipo, QGraphicsItem* parent)
    : QGraphicsRectItem(rect, parent), label(nullptr), tipo(tipo) {
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
    setPen(QPen(Qt::DashLine));
    setZValue(1);
}

void ZonaRectItem::setTexto(const QString& texto) {
    if (label) {
        scene()->removeItem(label);
        delete label;
    }
    label = new QGraphicsTextItem(texto, this);
    QFont font;
    font.setBold(true);
    font.setPointSize(14);
    label->setFont(font);
    label->setDefaultTextColor(Qt::black);

    QRectF r = rect();
    QRectF textRect = label->boundingRect();
    label->setPos(r.center().x() - textRect.width() / 2, r.center().y() - textRect.height() / 2);
    label->setZValue(2);
}

void ZonaRectItem::setColor(const QColor& color) {
    setBrush(QBrush(color));
    setPen(QPen(color.darker(), 2, Qt::DashLine));
}

void ZonaRectItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    Q_UNUSED(event);
    bool ok = false;
    QString nuevoTipo = QInputDialog::getItem(nullptr, "Cambiar tipo de zona", "Nuevo tipo:",
        { "inicio_ct", "inicio_tt", "zona_bombas" }, 0, false, &ok);
    if (ok) {
        tipo = nuevoTipo;
        setTexto(nuevoTipo == "inicio_ct" ? "CT" : (nuevoTipo == "inicio_tt" ? "TT" : "BOMBAS"));
        QColor color = (tipo == "inicio_ct") ? QColor(0, 0, 255, 50) :
                       (tipo == "inicio_tt") ? QColor(0, 255, 0, 50) :
                       QColor(255, 0, 0, 50);
        setColor(color);
        emit tipoZonaCambiado(this, tipo);
    }
}

void ZonaRectItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    QPointF pos = event->pos();
    QRectF r = rect();

    QRectF resizeZone(r.right() - resizeMargin, r.bottom() - resizeMargin, resizeMargin, resizeMargin);
    if (resizeZone.contains(pos)) {
        resizing = true;
        resizeStartPos = event->scenePos();
        originalRect = rect();
        event->accept();
    } else {
        QGraphicsRectItem::mousePressEvent(event);
    }
}

void ZonaRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (resizing) {
        QPointF delta = event->scenePos() - resizeStartPos;
        QRectF newRect = originalRect;
        newRect.setWidth(std::max(10.0, newRect.width() + delta.x()));
        newRect.setHeight(std::max(10.0, newRect.height() + delta.y()));
        setRect(newRect);
        setTexto(label->toPlainText());
    } else {
        QGraphicsRectItem::mouseMoveEvent(event);
    }
}

void ZonaRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (resizing) {
        resizing = false;
        event->accept();
    } else {
        QGraphicsRectItem::mouseReleaseEvent(event);
    }
}

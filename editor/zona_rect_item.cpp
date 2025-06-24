#include "zona_rect_item.h"
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QGraphicsScene>
#include <QInputDialog>

#define TEXTO_CT "CT"
#define TEXTO_TT "TT"
#define TEXTO_BOMBA "BOMBAS"

#define ZONA_CT "inicio_ct"
#define ZONA_TT "inicio_tt"
#define ZONA_BOMBA "zona_bombas"

ZonaRectItem::ZonaRectItem(const QRectF& rect, const QString& tipo, QUuid id, QGraphicsItem* parent)
    : QGraphicsRectItem(rect, parent), label(nullptr), tipo(tipo), id(id) {
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
    setPen(QPen(Qt::DashLine));
    setZValue(10);
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
    label->setZValue(11);
}

void ZonaRectItem::setColor(const QColor& color) {
    setBrush(QBrush(color));
    setPen(QPen(color.darker(), 2, Qt::DashLine));
}

void ZonaRectItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    Q_UNUSED(event);
    bool ok = false;
    QString nuevoTipo = QInputDialog::getItem(nullptr, "Cambiar tipo de zona", "Nuevo tipo:",
        { ZONA_CT, ZONA_TT, ZONA_BOMBA }, 0, false, &ok);
    if (!ok || nuevoTipo == tipo) return;

    if (tipo == ZONA_BOMBA && nuevoTipo != ZONA_BOMBA && imagenBombsite) {
        scene()->removeItem(imagenBombsite);
        delete imagenBombsite;
        imagenBombsite = nullptr;
    }

    tipo = nuevoTipo;

    if (tipo == ZONA_CT) {
        setTexto(TEXTO_CT);
        setColor(QColor(0, 0, 255, 50));
    } else if (tipo == ZONA_TT) {
        setTexto(TEXTO_TT);
        setColor(QColor(0, 255, 0, 50));
    } else if (tipo == ZONA_BOMBA) {
        setTexto(TEXTO_BOMBA);
        setColor(QColor(255, 0, 0, 50));

        if (!imagenBombsite)
            emit tipoZonaCambiado(this, tipo);
    }

    emit tipoZonaCambiado(this, tipo);
}


QVariant ZonaRectItem::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemPositionHasChanged) {
        if (imagenBombsite) {
            QPointF centro = this->sceneBoundingRect().center();
            QPointF nuevaPos = centro - QPointF(imagenBombsite->pixmap().width() / 2, imagenBombsite->pixmap().height() / 2);
            imagenBombsite->setPos(nuevaPos);
        }
        if (!resizing)
            emit zonaRectActualizado(this);
    }
    return QGraphicsRectItem::itemChange(change, value);
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
        emit zonaRectActualizado(this);
    } else {
        QGraphicsRectItem::mouseMoveEvent(event);
        emit zonaRectActualizado(this);
    }
}

void ZonaRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (resizing) {
        resizing = false;
        event->accept();

        if (rect() != originalRect) {
            emit zonaRectActualizado(this);
        }
    } else {
        QGraphicsRectItem::mouseReleaseEvent(event);
    }
}

void ZonaRectItem::setImagenBombsite(QGraphicsPixmapItem* imagen) {
    imagenBombsite = imagen;
}

QGraphicsPixmapItem* ZonaRectItem::getImagenBombsite() const {
    return imagenBombsite;
}



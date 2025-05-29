#include "draggable_label.h"
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QPixmap>
#include <QApplication>
#include <QBuffer>

DraggableLabel::DraggableLabel(const QPixmap& pixmap, QWidget* parent)
    : QLabel(parent),pixmapData(pixmap) {
        setPixmap(pixmap);
    }

void DraggableLabel::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        dragStartPosition = event->pos();
        emit dragStarted();
    }
    QLabel::mousePressEvent(event);
}

void DraggableLabel::mouseMoveEvent(QMouseEvent* event) {
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    QMimeData* mimeData = new QMimeData;

    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    pixmapData.save(&buffer, "PNG");
    mimeData->setData("application/x-pixmap", data);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmapData);
    drag->exec(Qt::CopyAction);
}

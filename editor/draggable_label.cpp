#include "draggable_label.h"
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QPixmap>
#include <QApplication>

DraggableLabel::DraggableLabel(const QString& imagePath, QWidget* parent)
    : QLabel(parent), path(imagePath) {}

void DraggableLabel::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton)
        dragStartPosition = event->pos();
    QLabel::mousePressEvent(event);
}

void DraggableLabel::mouseMoveEvent(QMouseEvent* event) {
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    QMimeData* mimeData = new QMimeData;
    mimeData->setText(path);  // Puedes usar otro formato según lo que necesites

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap());
    drag->exec(Qt::CopyAction);
}

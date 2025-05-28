#include "draggable_label.h"
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QPixmap>
#include <QApplication>

DraggableLabel::DraggableLabel(const QString& imagePath, QWidget* parent)
    : QLabel(parent), path(imagePath) {}

void DraggableLabel::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        dragStartPosition = event->pos();
        emit clicked(path);  // <--- emitir señal al hacer clic
    }
    QLabel::mousePressEvent(event);
}

void DraggableLabel::mouseMoveEvent(QMouseEvent* event) {
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    QMimeData* mimeData = new QMimeData;

    // Aquí cambio setText por setUrls
    QList<QUrl> urls;
    urls.append(QUrl::fromLocalFile(path));
    mimeData->setUrls(urls);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap());
    drag->exec(Qt::CopyAction);
}


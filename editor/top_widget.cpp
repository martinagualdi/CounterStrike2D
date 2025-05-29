#include "top_widget.h"
#include <QMimeData>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>

TopWidget::TopWidget(QWidget* parent) : QGraphicsView(parent), scene(new QGraphicsScene(this)) {
    setScene(scene);
    setAcceptDrops(true);
    setSceneRect(0, 0, 2000, 2000);
    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(Qt::white);
    setFocusPolicy(Qt::StrongFocus);
}

void TopWidget::setDropMode(DropMode mode) {
    currentMode = mode;
}

void TopWidget::drawBackground(QPainter* painter, const QRectF& rect) {
    QPen pen(Qt::lightGray);
    pen.setStyle(Qt::DotLine);
    painter->setPen(pen);

    for (int x = int(rect.left()) - (int(rect.left()) % gridSize); x < rect.right(); x += gridSize)
        painter->drawLine(x, rect.top(), x, rect.bottom());

    for (int y = int(rect.top()) - (int(rect.top()) % gridSize); y < rect.bottom(); y += gridSize)
        painter->drawLine(rect.left(), y, rect.right(), y);
}

void TopWidget::setBackgroundPath(const QString& path) {

    QPixmap tile(path);
    if (tile.isNull())
        return;

    scene->clear();

    QPixmap scaledTile = tile.scaled(gridSize, gridSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    scene->setSceneRect(0, 0, 2000, 2000);
    QRectF rect = scene->sceneRect();
    int width = int(rect.width());
    int height = int(rect.height());

    for (int y = 0; y < height; y += gridSize) {
        for (int x = 0; x < width; x += gridSize) {
            QGraphicsPixmapItem* item = new QGraphicsPixmapItem(scaledTile);
            item->setPos(x, y);
            item->setZValue(-1);
            scene->addItem(item);
        }
    }

    viewport()->update();
}

void TopWidget::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        QString path = event->mimeData()->urls().first().toLocalFile();
        QPixmap pix(path);
        if (!pix.isNull()) {
            currentDraggedPixmap = pix.scaled(gridSize, gridSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        event->acceptProposedAction();
    }
}

void TopWidget::dragMoveEvent(QDragMoveEvent* event) {
    if (!currentDraggedPixmap.isNull()) {
        QPointF scenePos = mapToScene(event->position().toPoint());
        int x = int(scenePos.x()) / gridSize * gridSize;
        int y = int(scenePos.y()) / gridSize * gridSize;

        if (!previewItem) {
            previewItem = new QGraphicsPixmapItem(currentDraggedPixmap);
            previewItem->setOpacity(0.5);
            previewItem->setZValue(10);
            scene->addItem(previewItem);
        }

        previewItem->setPos(x, y);
        event->acceptProposedAction();
    }
}

void TopWidget::dragLeaveEvent(QDragLeaveEvent* event) {
    Q_UNUSED(event);
    if (previewItem) {
        scene->removeItem(previewItem);
        delete previewItem;
        previewItem = nullptr;
    }
    currentDraggedPixmap = QPixmap();
}

void TopWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        for (QGraphicsItem* item : scene->selectedItems()) {
            if (item->zValue() > 0) {
                scene->removeItem(item);
                delete item;
            }
        }
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

void TopWidget::dropEvent(QDropEvent* event) {
    if (previewItem) {
        scene->removeItem(previewItem);
        delete previewItem;
        previewItem = nullptr;
    }

    currentDraggedPixmap = QPixmap();

    const QMimeData* mime = event->mimeData();
    if (!mime->hasUrls()) return;

    for (const QUrl& url : mime->urls()) {
        QString path = url.toLocalFile();
        QPixmap pix(path);
        if (pix.isNull()) continue;

        QPointF scenePos = mapToScene(event->position().toPoint());
        int x = int(scenePos.x()) / gridSize * gridSize;
        int y = int(scenePos.y()) / gridSize * gridSize;

        if (currentMode == DropMode::OBJETO) {
            auto* item = new QGraphicsPixmapItem(pix.scaled(gridSize, gridSize, Qt::KeepAspectRatio));
            item->setPos(x, y);
            item->setZValue(1);
            item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
            scene->addItem(item);
        } else if (currentMode == DropMode::FONDO) {
            setBackgroundPath(path);
        }
    }

    event->acceptProposedAction();
}


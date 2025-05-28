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
    setSceneRect(0, 0, 2000, 2000);  // Tamaño virtual del canvas
    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(Qt::white);
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
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void TopWidget::dropEvent(QDropEvent* event) {
    const QMimeData* mime = event->mimeData();
    if (!mime->hasUrls()) return;

    for (const QUrl& url : mime->urls()) {
        QString path = url.toLocalFile();
        QPixmap pix(path);
        qDebug() << "Intentando cargar imagen desde:" << path << "¿Es nulo?:" << pix.isNull();
        if (!pix.isNull()) {

            setBackgroundPath(path); 

            QPointF pos = mapToScene(event->position().toPoint());
            int x = int(pos.x()) / gridSize * gridSize;
            int y = int(pos.y()) / gridSize * gridSize;
            auto* item = new QGraphicsPixmapItem(pix.scaled(gridSize, gridSize, Qt::KeepAspectRatio));
            item->setPos(x, y);
            scene->addItem(item);
        }
    }
}


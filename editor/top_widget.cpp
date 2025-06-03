#include "top_widget.h"
#include "zona_rect_item.h"
#include <QMimeData>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QInputDialog>

TopWidget::TopWidget(QWidget* parent) : QGraphicsView(parent), scene(new QGraphicsScene(this)) {
    setScene(scene);
    setAcceptDrops(true);
    setSceneRect(0, 0, 2000, 2000);
    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(Qt::white);
    setFocusPolicy(Qt::StrongFocus);
}

QString TopWidget::getFondoPath() const {
    int idx = fondoPath.indexOf("/editor");
    if (idx != -1) {
        return fondoPath.mid(idx);
    }
    return fondoPath;
}

QList<ElementoMapa> TopWidget::getElementos() const {
    QList<ElementoMapa> elementos;
    for (QGraphicsItem* item : scene->items()) {
        if (item->zValue() > 0) {
            auto* pixmapItem = dynamic_cast<QGraphicsPixmapItem*>(item);
            if (pixmapItem) {
                QString imgPath = pixmapItem->data(0).toString();
                QString tipo = pixmapItem->data(1).toString();

                if (!imgPath.isEmpty()) {
                    ElementoMapa elem;
                    elem.path = imgPath;
                    elem.posicion = pixmapItem->pos();
                    elem.tipo = tipo;

                    elementos.append(elem);
                }
            }
        }
    }
    return elementos;
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

    fondoPath = path;

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
    const QMimeData* mime = event->mimeData();

    if (mime->hasUrls()) {
        QString path = mime->urls().first().toLocalFile();
        QPixmap pix = filtrarFondo(path, gridSize);
        if (!pix.isNull()) {
            currentDraggedPixmap = pix;
            event->acceptProposedAction();
        }
    }
}

void TopWidget::dragMoveEvent(QDragMoveEvent* event) {
    if (!currentDraggedPixmap.isNull()) {
        QPointF scenePos = mapToScene(event->position().toPoint());
        int x = int(scenePos.x()) / gridSize * gridSize;
        int y = int(scenePos.y()) / gridSize * gridSize;

        if (!previewItem) {
            QPixmap filteredPixmap = currentDraggedPixmap;
            previewItem = new QGraphicsPixmapItem(filteredPixmap);
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

    for (const QUrl& url : mime->urls()) {
        QString path = url.toLocalFile();
        QPixmap pix = filtrarFondo(path, gridSize);
        if (pix.isNull()) continue;

        QPointF scenePos = mapToScene(event->position().toPoint());
        int x = int(scenePos.x()) / gridSize * gridSize;
        int y = int(scenePos.y()) / gridSize * gridSize;

        if (currentMode == DropMode::OBJETO) {
            auto* item = new QGraphicsPixmapItem(pix);
            item->setPos(x, y);
            item->setZValue(1);
            item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

            QString tipo = "otros";
            if (path.contains("plantacion_bombas"))
                tipo = "bombsite";
            else if (path.contains("proteccion_disparos"))
                tipo = "proteccion";
            else if (path.contains("spawns"))
                tipo = "spawn";
            else if(path.contains("weapons"))
                tipo = "arma";
            else if(path.contains("obstaculo"))
                tipo = "obstaculo";
            else if(path.contains("piso"))
                tipo = "piso";

            item->setData(0, path);
            item->setData(1, tipo);
            scene->addItem(item);
        } else if (currentMode == DropMode::FONDO) {
            QString rutaRelativa = path.mid(path.indexOf("/editor"));
            setBackgroundPath(rutaRelativa);
        }
    }

    event->acceptProposedAction();
}

void TopWidget::agregarElemento(const QString& path, int x, int y) {
    QPixmap pixmap = filtrarFondo(path, gridSize);
    if (!pixmap.isNull()) {
        auto* item = new QGraphicsPixmapItem(pixmap);
        item->setPos(x, y);
        item->setZValue(1);
        item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

        QString tipo = "otros";
        if (path.contains("plantacion_bombas"))
            tipo = "bombsite";
        else if (path.contains("proteccion_disparos"))
            tipo = "proteccion";
        else if (path.contains("spawns"))
            tipo = "spawn";
        else if(path.contains("weapons"))
            tipo = "arma";
        else if(path.contains("obstaculo"))
            tipo = "obstaculo";
        else if(path.contains("piso"))
            tipo = "piso";
        item->setData(0, path);
        item->setData(1, tipo);

        scene->addItem(item);
    }
}

void TopWidget::mousePressEvent(QMouseEvent* event) {
    if (currentMode == DropMode::ZONA_INICIO) {
        zonaStartPoint = mapToScene(event->pos());
        if (zonaPreview) {
            scene->removeItem(zonaPreview);
            delete zonaPreview;
            zonaPreview = nullptr;
        }
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void TopWidget::mouseMoveEvent(QMouseEvent* event) {
    if (currentMode == DropMode::ZONA_INICIO && event->buttons() & Qt::LeftButton) {
        QPointF currentPos = mapToScene(event->pos());
        QRectF rect(zonaStartPoint, currentPos);
        if (!zonaPreview) {
            zonaPreview = scene->addRect(rect.normalized(), QPen(Qt::blue, 2, Qt::DashLine));
        } else {
            zonaPreview->setRect(rect.normalized());
        }
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void TopWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (currentMode == DropMode::ZONA_INICIO && zonaPreview) {
        bool ok = false;
        QString tipo = QInputDialog::getItem(
            this, "Tipo de zona", "¿Qué zona estás marcando?",
            { "inicio_ct", "inicio_tt", "zona_bombas" }, 0, false, &ok
        );

        if (ok) {
            QRectF rect = zonaPreview->rect();

            auto* zonaItem = new ZonaRectItem(rect, tipo);
            connect(zonaItem, &ZonaRectItem::tipoZonaCambiado, this, [this](ZonaRectItem* item, const QString& nuevoTipo) {
                for (ZonaMapa& zona : zonasInicio) {
                    if (zona.rect == item->rect()) {
                        zona.tipo = nuevoTipo;
                        break;
                    }
                }
            });
            
            QColor color;
            QString texto;

            if (tipo == "inicio_ct") {
                color = QColor(0, 0, 255, 50);
                texto = "CT";
            } else if (tipo == "inicio_tt") {
                color = QColor(0, 255, 0, 50);
                texto = "TT";
            } else if (tipo == "zona_bombas") {
                color = QColor(255, 0, 0, 50);
                texto = "BOMBAS";
            }

            zonaItem->setColor(color);
            zonaItem->setTexto(texto);
            scene->addItem(zonaItem);

            ZonaMapa zona;
            zona.rect = rect;
            zona.tipo = tipo;
            zonasInicio.append(zona);
            emit zonaCreada(tipo, rect);

            currentMode = DropMode::OBJETO;
            scene->removeItem(zonaPreview);
            delete zonaPreview;
            zonaPreview = nullptr;
        } else {
            scene->removeItem(zonaPreview);
            delete zonaPreview;
            zonaPreview = nullptr;
        }
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void TopWidget::agregarZona(const QRectF& rect, const QString& tipo) {
    QColor color;
    QString texto;

    if (tipo == "inicio_ct") {
        color = QColor(0, 0, 255, 50);
        texto = "CT";
    } else if (tipo == "inicio_tt") {
        color = QColor(0, 255, 0, 50);
        texto = "TT";
    } else if (tipo == "zona_bombas") {
        color = QColor(255, 0, 0, 50);
        texto = "BOMBAS";
    }

    auto* zonaItem = new ZonaRectItem(rect, tipo);
    zonaItem->setColor(color);
    zonaItem->setTexto(texto);
    scene->addItem(zonaItem);

    ZonaMapa zona;
    zona.rect = rect;
    zona.tipo = tipo;
    zonasInicio.append(zona);
}

QPixmap TopWidget::filtrarFondo(const QString& path, int gridSize) {
    QImage img(path);
    if (img.isNull())
        return QPixmap();

    img = img.convertToFormat(QImage::Format_ARGB32);
    QRgb magenta = qRgb(255, 0, 255);

    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            QRgb pixel = img.pixel(x, y);
            if (pixel == magenta) {
                img.setPixelColor(x, y, QColor(0, 0, 0, 0));
            }
        }
    }

    return QPixmap::fromImage(img).scaled(gridSize, gridSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}



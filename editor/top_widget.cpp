#include "top_widget.h"
#include "zona_rect_item.h"
#include "grid_pixmap_item.h"
#include <QMimeData>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QInputDialog>
#include <QMessageBox>
#include <QCoreApplication>

TopWidget::TopWidget(QWidget* parent) : QGraphicsView(parent){
    setScene(new QGraphicsScene(this));
    setAcceptDrops(true);
    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(Qt::white);
    setFocusPolicy(Qt::StrongFocus);
}

int TopWidget::pedirDimensionMapa(const QString& titulo, const QString& label, int valorPorDefecto) {
    QStringList opciones;
    for (int i = 640; i <= 2048; i += 32) {
        opciones << QString::number(i);
    }

    QInputDialog dialog(this);
    dialog.setComboBoxItems(opciones);
    dialog.setWindowTitle(titulo);
    dialog.setLabelText(label);
    dialog.setOption(QInputDialog::UseListViewForComboBoxItems); // activa scroll automático
    dialog.setTextValue(QString::number(valorPorDefecto));

    // Estilo opcional (como tenías antes)
    QString estilo = R"(
        QInputDialog {
            background-color: #2c2c2c;
            font-family: Arial;
            font-size: 14px;
        }
        QLabel {
            color: white;
        }
        QComboBox, QLineEdit {
            background-color: #444;
            color: white;
            border: 1px solid #888;
            border-radius: 5px;
            padding: 4px;
        }
        QScrollBar:vertical {
            background: #333;
            width: 12px;
        }
        QScrollBar::handle:vertical {
            background: #888;
            min-height: 20px;
        }
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0px;
        }
    )";
    dialog.setStyleSheet(estilo);

    if (dialog.exec() == QDialog::Accepted) {
        return dialog.textValue().toInt();
    }

    return -1;
}

void TopWidget::preguntarTamanioMapa() {
    if (tamanioEstablecidoDesdeYAML) return;

    int ancho = pedirDimensionMapa("Tamaño del mapa", "Ancho del mapa (px):", 2048);
    if (ancho == -1) {
        maxAncho = maxAlto = 2048;
        setSceneRect(0, 0, 2048, 2048);
        return;
    }

    int alto = pedirDimensionMapa("Tamaño del mapa", "Alto del mapa (px):", 2048);
    if (alto == -1) {
        maxAncho = maxAlto = 2048;
        setSceneRect(0, 0, 2048, 2048);
        return;
    }

    maxAncho = ancho;
    maxAlto = alto;
    setSceneRect(0, 0, ancho, alto);
}

void TopWidget::activarPincelPiso(){
    pincelActivo = true;
}

bool TopWidget::estaPincelActivo() const {
    return pincelActivo;
}

void TopWidget::setTamanioMapaDesdeYAML(int ancho, int alto) {
    setSceneRect(0, 0, ancho, alto);
    tamanioEstablecidoDesdeYAML = true;
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
    for (QGraphicsItem* item : scene()->items()) {
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
                    QPixmap pix = pixmapItem->pixmap();
                    elem.ancho = pix.width();
                    elem.alto = pix.height();

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

    scene()->clear();

    QPixmap scaledTile = tile.scaled(gridSize, gridSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    scene()->setSceneRect(0, 0, maxAncho, maxAlto);
    QRectF rect = scene()->sceneRect();
    int width = int(rect.width());
    int height = int(rect.height());

    for (int y = 0; y < height; y += gridSize) {
        for (int x = 0; x < width; x += gridSize) {
            QGraphicsPixmapItem* item = new QGraphicsPixmapItem(scaledTile);
            item->setPos(x, y);
            item->setZValue(-1);
            scene()->addItem(item);
        }
    }

    viewport()->update();
}

void TopWidget::dragEnterEvent(QDragEnterEvent* event) {
    const QMimeData* mime = event->mimeData();

    if (mime->hasUrls()) {
        QString path = mime->urls().first().toLocalFile();
        QPixmap pix = filtrarFondo(path);
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
            scene()->addItem(previewItem);
        }

        previewItem->setPos(x, y);
        event->acceptProposedAction();
    }
}

void TopWidget::dragLeaveEvent(QDragLeaveEvent* event) {
    Q_UNUSED(event);
    if (previewItem) {
        scene()->removeItem(previewItem);
        delete previewItem;
        previewItem = nullptr;
    }
    currentDraggedPixmap = QPixmap();
}

void TopWidget::eliminarZona(ZonaRectItem* zonaItem) {
    if (!zonaItem) return;

    auto it = std::remove_if(zonasInicio.begin(), zonasInicio.end(),
        [zonaItem](const ZonaMapa& z) {
            return z.id == zonaItem->getId();
        });
    zonasInicio.erase(it, zonasInicio.end());

    scene()->removeItem(zonaItem);
    delete zonaItem;
}

void TopWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        for (QGraphicsItem* item : scene()->selectedItems()) {
            if (item->zValue() > 0) {
                if (auto* zonaItem = dynamic_cast<ZonaRectItem*>(item)) {
                    eliminarZona(zonaItem);
                } else {
                    scene()->removeItem(item);
                    delete item;
                }
            }
        }
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

void TopWidget::dropEvent(QDropEvent* event) {
    if (previewItem) {
        scene()->removeItem(previewItem);
        delete previewItem;
        previewItem = nullptr;
    }

    currentDraggedPixmap = QPixmap();

    const QMimeData* mime = event->mimeData();

    for (const QUrl& url : mime->urls()) {
        QString path = url.toLocalFile();

        QPixmap pix = filtrarFondo(path);

        if (pix.isNull()) continue;

        QPointF scenePos = mapToScene(event->position().toPoint());
        int x = int(scenePos.x()) / gridSize * gridSize;
        int y = int(scenePos.y()) / gridSize * gridSize;

        if (currentMode == DropMode::OBJETO) {
            auto* item = new GridPixmapItem(pix, gridSize);
            item->setPos(x, y);
            item->setZValue(1);

            QString tipo = "otros";
            if (path.contains("plantacion_bombas"))
                tipo = "bombsite";
            else if (path.contains("spawns"))
                tipo = "spawn";
            else if(path.contains("weapons"))
                tipo = "arma";
            else if(path.contains("obstaculo")||path.contains("proteccion_disparos"))

                tipo = "obstaculo";
            else if(path.contains("piso"))
                tipo = "piso";

            item->setData(0, path);
            item->setData(1, tipo);
            item->setData(2, pix.width());
            item->setData(3, pix.height());

            scene()->addItem(item);
        } else if (currentMode == DropMode::FONDO) {
            QString rutaRelativa = path.mid(path.indexOf("/editor"));
            setBackgroundPath(rutaRelativa);
        }
    }

    event->acceptProposedAction();
}

void TopWidget::pintarPisoEnPosicion(const QPoint& pos) {
    QPointF scenePos = mapToScene(pos);
    int x = int(scenePos.x()) / gridSize * gridSize;
    int y = int(scenePos.y()) / gridSize * gridSize;
    QPoint celda(x, y);

    if (celdasPintadas.contains(celda))
        return;

    celdasPintadas.insert(celda);

    QPixmap pix(currentPisoPath);
    if (!pix.isNull()) {
        auto* item = new GridPixmapItem(pix, gridSize);
        item->setPos(x, y);
        item->setZValue(1);
        item->setData(0, currentPisoPath);
        item->setData(1, "piso");
        item->setData(2, pix.width());
        item->setData(3, pix.height());
        scene()->addItem(item);
    }
}

void TopWidget::setPisoPath(const QString& path) {
    currentPisoPath = path;
}

void TopWidget::mousePressEvent(QMouseEvent* event) {
    if (currentMode == DropMode::ZONA_INICIO) {
        zonaStartPoint = mapToScene(event->pos());
        if (zonaPreview) {
            scene()->removeItem(zonaPreview);
            delete zonaPreview;
            zonaPreview = nullptr;
        }
    } else if (currentMode == DropMode::PINCEL_PISO && !currentPisoPath.isEmpty()) {
        mousePressed = true;
        celdasPintadas.clear();
        pintarPisoEnPosicion(event->pos());
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void TopWidget::mouseMoveEvent(QMouseEvent* event) {
    if (currentMode == DropMode::ZONA_INICIO && event->buttons() & Qt::LeftButton) {
        QPointF currentPos = mapToScene(event->pos());
        QRectF rect(zonaStartPoint, currentPos);
        if (!zonaPreview) {
            zonaPreview = scene()->addRect(rect.normalized(), QPen(Qt::blue, 2, Qt::DashLine));
            zonaPreview->setZValue(100);
        } else {
            zonaPreview->setRect(rect.normalized());
        }
    } else if(mousePressed && currentMode == DropMode::PINCEL_PISO && !currentPisoPath.isEmpty()){
        pintarPisoEnPosicion(event->pos());
    }else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void TopWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (currentMode == DropMode::ZONA_INICIO && zonaPreview) {
        QString tipo = pedirTipoZona();
        if (tipo.isEmpty()) {
            limpiarPreviewZona();
            return;
        }

        if (!validarCantidadZonas(tipo)) {
            limpiarPreviewZona();
            return;
        }

        QRectF rect = zonaPreview->rect();

        QUuid uuid = QUuid::createUuid();
        auto* zonaItem = new ZonaRectItem(rect, tipo, uuid);
        conectarCambioTipo(zonaItem);
        conectarActualizacionRect(zonaItem);

        QColor color = colorParaTipo(tipo);
        QString texto = textoParaTipo(tipo);

        zonaItem->setColor(color);
        zonaItem->setTexto(texto);
        scene()->addItem(zonaItem);

        if (tipo == "zona_bombas")
            agregarImagenBomba(rect);

        ZonaMapa zona;
        zona.rect = rect;
        zona.tipo = tipo;
        zona.id = zonaItem->getId();
        zonasInicio.append(zona);

        emit zonaCreada(tipo, rect);

        currentMode = DropMode::OBJETO;
        limpiarPreviewZona();
    } else if (currentMode == DropMode::PINCEL_PISO){
        pincelActivo = false;
        mousePressed = false;
        celdasPintadas.clear();
        setDropMode(DropMode::OBJETO);
    } else{
        QGraphicsView::mouseReleaseEvent(event);
    }
}

QString TopWidget::pedirTipoZona() {
    bool ok = false;
    QString tipo = QInputDialog::getItem(
        this, "Tipo de zona", "¿Qué zona estás marcando?",
        { "inicio_ct", "inicio_tt", "zona_bombas" }, 0, false, &ok
    );
    return ok ? tipo : "";
}

bool TopWidget::validarCantidadZonas(const QString& tipo) {
    int cantidad = std::count_if(zonasInicio.begin(), zonasInicio.end(), [&](const ZonaMapa& z) {
        return z.tipo == tipo;
    });

    if ((tipo == "inicio_ct" || tipo == "inicio_tt") && cantidad >= 1) {
        QMessageBox::warning(this, "Zona duplicada", "Ya existe una zona de tipo " + tipo + ".");
        return false;
    }

    if (tipo == "zona_bombas" && cantidad >= 2) {
        QMessageBox::warning(this, "Zona duplicada", "Solo se permiten dos zonas de bombas.");
        return false;
    }

    return true;
}

void TopWidget::conectarCambioTipo(ZonaRectItem* zonaItem) {
    connect(zonaItem, &ZonaRectItem::tipoZonaCambiado, this,
        [this](ZonaRectItem* item, const QString& nuevoTipo) {
            for (ZonaMapa& zona : zonasInicio) {
                if (zona.rect == item->rect()) {
                    zona.tipo = nuevoTipo;
                    break;
                }
            }
    });

}

void TopWidget::conectarActualizacionRect(ZonaRectItem* item) {
    connect(item, &ZonaRectItem::zonaRectActualizado, this,
        [this](ZonaRectItem* source) {
            for (ZonaMapa& zona : zonasInicio) {
                if (zona.id == source->getId()) {
                    zona.rect = source->mapRectToScene(source->rect());
                    break;
                }
            }
        });
}

QColor TopWidget::colorParaTipo(const QString& tipo) const {
    if (tipo == "inicio_ct") return QColor(0, 0, 255, 50);
    if (tipo == "inicio_tt") return QColor(0, 255, 0, 50);
    if (tipo == "zona_bombas") return QColor(255, 0, 0, 50);
    return QColor(100, 100, 100, 50);  // fallback
}

QString TopWidget::textoParaTipo(const QString& tipo) const {
    if (tipo == "inicio_ct") return "CT";
    if (tipo == "inicio_tt") return "TT";
    return "";
}

void TopWidget::agregarImagenBomba(const QRectF& rect) {
    int cantidad = std::count_if(zonasInicio.begin(), zonasInicio.end(), [](const ZonaMapa& z) {
        return z.tipo == "zona_bombas";
    });

    QString basePath = QCoreApplication::applicationDirPath();
    QString imagen = basePath + ((cantidad == 0)
        ? "/editor/gfx/plantacion_bombas/plantacion1.png"
        : "/editor/gfx/plantacion_bombas/plantacion2.png");

    QPixmap pix(imagen);
    if (!pix.isNull()) {
        QPointF centro = rect.center();
        QPointF pos = centro - QPointF(pix.width() / 2, pix.height() / 2);

        auto* imagenItem = new QGraphicsPixmapItem(pix);
        imagenItem->setPos(pos);
        imagenItem->setZValue(0.5);
        scene()->addItem(imagenItem);
    }
}

void TopWidget::limpiarPreviewZona() {
    scene()->removeItem(zonaPreview);
    delete zonaPreview;
    zonaPreview = nullptr;
}

void TopWidget::agregarElemento(const QString& path, int x, int y) {
    QPixmap pixmap = filtrarFondo(path);
    if (!pixmap.isNull()) {
        auto* item = new GridPixmapItem(pixmap, gridSize);
        item->setPos(x, y);
        item->setZValue(1);

        QString tipo = "otros";
        if (path.contains("plantacion_bombas"))
            tipo = "bombsite";
        else if (path.contains("spawns"))
            tipo = "spawn";
        else if(path.contains("weapons"))
            tipo = "arma";
        else if(path.contains("obstaculo")||path.contains("proteccion_disparos"))
            tipo = "obstaculo";
        else if(path.contains("piso"))
            tipo = "piso";
        item->setData(0, path);
        item->setData(1, tipo);
        item->setData(2, pixmap.width());
        item->setData(3, pixmap.height());

        scene()->addItem(item);
    }
}

void TopWidget::agregarZona(const QRectF& rect, const QString& tipo, const QUuid& id) {
    QColor color;
    QString texto;
    int cantidadExistente = 0;
    for (const ZonaMapa& zona : zonasInicio) {
                if (zona.tipo == tipo)
                    cantidadExistente++;
    }

    if (tipo == "inicio_ct") {
        color = QColor(0, 0, 255, 50);
        texto = "CT";
    } else if (tipo == "inicio_tt") {
        color = QColor(0, 255, 0, 50);
        texto = "TT";
    } else if (tipo == "zona_bombas") {
        color = QColor(255, 0, 0, 50);
    }

    auto* zonaItem = new ZonaRectItem(rect, tipo, id);
    conectarCambioTipo(zonaItem);
    conectarActualizacionRect(zonaItem);
    zonaItem->setColor(color);
    zonaItem->setTexto(texto);

    if (tipo == "zona_bombas") {
        agregarImagenBomba(rect);
    }

    scene()->addItem(zonaItem);
    ZonaMapa zona;
    zona.rect = rect;
    zona.tipo = tipo;
    zona.id = zonaItem->getId();
    zonasInicio.append(zona);
}

QPixmap TopWidget::filtrarFondo(const QString& path) {
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

    return QPixmap::fromImage(img);
}

QImage TopWidget::generarMiniatura(const QRectF& area, const QSize& destino) {
    QRectF renderArea = area.isNull() ? this->scene()->sceneRect() : area;

    QImage imagen(renderArea.size().toSize(), QImage::Format_ARGB32);
    imagen.fill(Qt::transparent);

    QPainter painter(&imagen);
    this->scene()->render(&painter, QRectF(QPointF(0, 0), renderArea.size()), renderArea);
    return imagen.scaled(destino, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}


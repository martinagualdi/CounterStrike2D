#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

enum class DropMode {
    FONDO,
    OBJETO,
    ZONA_INICIO
};

struct ElementoMapa {
    QString path;
    QPointF posicion;
    QString tipo;
};

struct ZonaMapa {
    QRectF rect;     // La zona rectangular
    QString tipo;    // "inicio_ct" o "inicio_tt"
};

class TopWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit TopWidget(QWidget* parent = nullptr);
    void setBackgroundPath(const QString& path);
    void setDropMode(DropMode mode);
    QString getFondoPath() const;
    QList<ElementoMapa> getElementos() const;
    void agregarElemento(const QString& path, int x, int y);
    QList<ZonaMapa> getZonas() const { return zonasInicio; }
    void agregarZona(const QRectF& rect, const QString& tipo);

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void keyPressEvent(QKeyEvent* event)  override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    QGraphicsScene* scene;
    QPixmap backgroundPixmap;
    int gridSize = 64;
    QGraphicsPixmapItem* previewItem = nullptr; 
    QPixmap currentDraggedPixmap; 
    DropMode currentMode = DropMode::OBJETO;
    QString fondoPath;
    QList<ZonaMapa> zonasInicio;
    QPointF zonaStartPoint;
    QGraphicsRectItem* zonaPreview = nullptr;

signals:
    void zonaCreada(const QString& tipo, const QRectF& zona);
};

#ifndef TOP_WIDGET_H
#define TOP_WIDGET_H

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
    int ancho;
    int alto;
};

struct ZonaMapa {
    QRectF rect;
    QString tipo;
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
    void preguntarTamanioMapa();
    void setTamanioMapaDesdeYAML(int ancho, int alto);
    int getMaxAncho() const { return maxAncho; }
    int getMaxAlto() const { return maxAlto; }

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
    QPixmap filtrarFondo(const QString& path);

private:
    QPixmap backgroundPixmap;
    int gridSize = 32;
    QGraphicsPixmapItem* previewItem = nullptr; 
    QPixmap currentDraggedPixmap; 
    DropMode currentMode = DropMode::OBJETO;
    QString fondoPath;
    QList<ZonaMapa> zonasInicio;
    QPointF zonaStartPoint;
    QGraphicsRectItem* zonaPreview = nullptr;
    bool tamanioEstablecidoDesdeYAML = false;
    int maxAncho = 2048;
    int maxAlto = 2048;

    QString pedirTipoZona();
    bool validarCantidadZonas(const QString& tipo);
    void conectarCambioTipo(class ZonaRectItem* zonaItem);
    QColor colorParaTipo(const QString& tipo) const;
    QString textoParaTipo(const QString& tipo) const;
    void agregarImagenBomba(const QRectF& rect);
    void limpiarPreviewZona();

signals:
    void zonaCreada(const QString& tipo, const QRectF& zona);
};

#endif // TOP_WIDGET_H
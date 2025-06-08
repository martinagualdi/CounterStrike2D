#ifndef TOP_WIDGET_H
#define TOP_WIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QUuid>

enum class DropMode {
    FONDO,
    OBJETO,
    ZONA_INICIO,
    PINCEL_PISO
};

struct ElementoMapa {
    QString path;
    QPointF posicion;
    QString tipo;
    int ancho;
    int alto;
};

struct ZonaMapa {
    QUuid id;
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
    void agregarZona(const QRectF& rect, const QString& tipo, const QUuid& id);
    void preguntarTamanioMapa();
    void setTamanioMapaDesdeYAML(int ancho, int alto);
    int getMaxAncho() const { return maxAncho; }
    int getMaxAlto() const { return maxAlto; }
    void setPisoPath(const QString& path);
    void activarPincelPiso();
    bool estaPincelActivo() const;

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
    void pintarPisoEnPosicion(const QPoint& pos);

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
    QString currentPisoPath;
    bool mousePressed = false;
    QSet<QPoint> celdasPintadas;
    bool pincelActivo = false;
    
    int maxAncho = 2048;
    int maxAlto = 2048;

    QString pedirTipoZona();
    bool validarCantidadZonas(const QString& tipo);
    void conectarCambioTipo(class ZonaRectItem* zonaItem);
    void conectarActualizacionRect(ZonaRectItem* item);
    QColor colorParaTipo(const QString& tipo) const;
    QString textoParaTipo(const QString& tipo) const;
    void agregarImagenBomba(const QRectF& rect);
    void limpiarPreviewZona();
    void eliminarZona(ZonaRectItem* zonaItem);
    int pedirDimensionMapa(const QString& titulo, const QString& label, int valorPorDefecto);

signals:
    void zonaCreada(const QString& tipo, const QRectF& zona);
};

#endif // TOP_WIDGET_H
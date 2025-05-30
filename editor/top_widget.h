#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

enum class DropMode {
    FONDO,
    OBJETO
};

class TopWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit TopWidget(QWidget* parent = nullptr);
    void setBackgroundPath(const QString& path);
    void setDropMode(DropMode mode);
    QString getFondoPath() const;
    QList<QPair<QString, QPointF>> getElementos() const;


protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void keyPressEvent(QKeyEvent* event)  override;

private:
    QGraphicsScene* scene;
    QPixmap backgroundPixmap;
    int gridSize = 64;
    QGraphicsPixmapItem* previewItem = nullptr; 
    QPixmap currentDraggedPixmap; 
    DropMode currentMode = DropMode::OBJETO;
    QString fondoPath;
};

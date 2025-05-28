#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

class TopWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit TopWidget(QWidget* parent = nullptr);
    void setBackgroundPath(const QString& path);

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    QGraphicsScene* scene;
    QPixmap backgroundPixmap;
    int gridSize = 32;
};

#ifndef DRAGGABLE_LABEL_H
#define DRAGGABLE_LABEL_H

#include <QLabel>
#include <QString>

class DraggableLabel : public QLabel {
    Q_OBJECT

public:
    explicit DraggableLabel(const QPixmap& pixmap, QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

signals:
    void dragStarted();

private:
    QPixmap pixmapData; 
    QPoint dragStartPosition;
};

#endif // DRAGGABLE_LABEL_H

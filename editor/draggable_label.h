#ifndef DRAGGABLE_LABEL_H
#define DRAGGABLE_LABEL_H

#include <QLabel>
#include <QString>

class DraggableLabel : public QLabel {
    Q_OBJECT

public:
    explicit DraggableLabel(const QString& imagePath, QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

signals:
    void clicked(const QString& path);

private:
    QString path;
    QPoint dragStartPosition;
};

#endif // DRAGGABLE_LABEL_H

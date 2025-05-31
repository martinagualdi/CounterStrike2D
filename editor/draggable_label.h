#ifndef DRAGGABLE_LABEL_H
#define DRAGGABLE_LABEL_H

#include <QLabel>
#include <QString>

class DraggableLabel : public QLabel {
    Q_OBJECT

public:
    explicit DraggableLabel(const QString& imagePath, QWidget* parent = nullptr);
    void setTipo(const QString& tipo_) { tipo = tipo_; }
    QString getTipo() const { return tipo; }

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

signals:
    void dragStarted(const QString& path);

private:
    QString path;
    QPoint dragStartPosition;
    QString tipo = "otros";
};

#endif // DRAGGABLE_LABEL_H

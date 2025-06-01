#include "clickable_label.h"
#include <QMouseEvent>

ClickableLabel::ClickableLabel(const QString& imagePath, QWidget* parent)
    : QLabel(parent), path(imagePath) {}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(path);
    }
    QLabel::mousePressEvent(event);
}

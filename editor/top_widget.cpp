#include "top_widget.h"

#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QPixmap>
#include <QPalette>
#include <QBrush>
#include <QDebug>

TopWidget::TopWidget(QWidget* parent) : QWidget(parent) {
    setAcceptDrops(true);
}

void TopWidget::dragEnterEvent(QDragEnterEvent* event) {
    /*QPalette pal = this->palette();
    pal.setColor(QPalette::Window, Qt::yellow);
    this->setPalette(pal);
    this->setAutoFillBackground(true);
    update();*/

    event->acceptProposedAction();
}

void TopWidget::setBackgroundPath(const QString& path) {
    backgroundPath = path;
    QPixmap bg(path);
    if (!bg.isNull()) {
        QPalette pal = palette();
        pal.setBrush(QPalette::Window, QBrush(bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
        setAutoFillBackground(true);
        setPalette(pal);
        update();
    } else {
        qDebug() << "No se pudo cargar la imagen:" << path;
    }
}

void TopWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (!backgroundPath.isEmpty()) {
        QPixmap bg(backgroundPath);
        if (!bg.isNull()) {
            QPalette pal = palette();
            pal.setBrush(QPalette::Window, QBrush(bg.scaled(size(), Qt::KeepAspectRatioByExpanding)));
            setPalette(pal);
            setAutoFillBackground(true);
            update();
        }
    }
}


void TopWidget::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty()) {
            QString filePath = urls.first().toLocalFile();
            setBackgroundPath(filePath);
        }
    }
}



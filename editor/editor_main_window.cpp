#include "editor_main_window.h"
#include "draggable_label.h" 

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QPixmap>
#include <QDir>
#include <QPalette>
#include <QBrush>
#include <QPushButton>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent) : QWidget(parent) {
    // Fondo blanco inicial
    setStyleSheet("background-color: white;");

    // Layout principal vertical
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Espacio superior (puede ser para mapa o vacío)
    QWidget* topWidget = new QWidget;
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(topWidget);

    // Scroll area para los thumbnails (parte inferior)
    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setFixedHeight(120);  // Altura para thumbnails
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);

    QWidget* container = new QWidget;
    QHBoxLayout* hLayout = new QHBoxLayout(container);
    hLayout->setSpacing(5);
    hLayout->setContentsMargins(5, 5, 5, 5);

    // Cargar imágenes desde el directorio
    QDir dir("editor/gfx/backgrounds/");
    QStringList images = dir.entryList(QStringList() << "*.png" << "*.jpg" << "*.bmp", QDir::Files);

    for (const QString& imgName : images) {
        QString fullPath = dir.absoluteFilePath(imgName);
        QPixmap pix(fullPath);
        DraggableLabel* label = new DraggableLabel(fullPath);
        label->setPixmap(pix.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        label->setFixedSize(100, 100);
        hLayout->addWidget(label);

        // opcional: todavía podés permitir hacer clic si querés
        connect(label, &DraggableLabel::clicked, this, [this](const QString& path) {
            QPixmap bg(path);
            QPalette pal = this->palette();
            pal.setBrush(QPalette::Window, QBrush(bg.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
            this->setAutoFillBackground(true);
            this->setPalette(pal);
            this->update();
        });
    }

    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);
    setAcceptDrops(true);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasImage() || event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event) {
    const QMimeData* mimeData = event->mimeData();

    // Leer la ruta como texto (porque DraggableLabel usa setText)
    if (mimeData->hasText()) {
        QString filePath = mimeData->text();
        QPixmap bg(filePath);
        if (!bg.isNull()) {
            QPalette pal = this->palette();
            pal.setBrush(QPalette::Window, QBrush(bg.scaled(this->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)));
            this->setAutoFillBackground(true);
            this->setPalette(pal);
            this->update();
        }
    }
}